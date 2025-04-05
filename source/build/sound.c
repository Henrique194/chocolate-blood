#include <stdio.h>
#include <SDL3/SDL.h>
#include "sound.h"
#include "opl3.h"

static SDL_AudioDeviceID device;
static SDL_AudioStream* stream_blaster;
static int sound_init;
static int sound_rate;
static SDL_AudioSpec spec_main;


static opl3_chip opl3;
static SDL_AudioStream* stream_adlib;
#define ADLIB_BUFFER_SIZE_SAMPLES 16384
static int16_t stream_adlib_buffer[ADLIB_BUFFER_SIZE_SAMPLES * 2];
static int opl3_address;
static int opl3_newm;
static int music_pit_counter;
static int music_pit_divider;
static void (*music_pit_callback)();
static char* blaster_dma_buffer;
static char* blaster_dma_buffer2;
static int blaster_dma_buffer_size;
static int blaster_16bit;
static int blaster_stereo;
static int blaster_rate;
static int blaster_dma_count;
static int blaster_dma_count_init;
static int blaster_dma_ptr;
static int blaster_dma_ptr_init;
static int blaster_dma_running;
static int blaster_dma_auto;
static int blaster_irq_count;
static int blaster_irq_count_init;
static void (*blaster_irq_callback)();
static double blaster_pit_counter;
static int blaster_pit_divider;
static void (*blaster_pit_callback)();
#define BLASTER_BUFFER_SIZE 65536
static char stream_blaster_buffer[BLASTER_BUFFER_SIZE];

static SDL_AudioStream* stream_blaster;

int blaster_type = BLASTER_TYPE_16;

SDL_Mutex *snd_mutex;

#define MUSIC_GAIN 2.f

static bool Adlib_Callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
	if (stream != stream_adlib)
		return false;

	int samples = total_amount / 4;
	int send = 0;

	while (send < samples)
	{
		int count = samples - send;
		if (count > ADLIB_BUFFER_SIZE_SAMPLES)
			count = ADLIB_BUFFER_SIZE_SAMPLES;

		if (music_pit_divider)
		{
			int due = (-music_pit_counter + 23) / 24;
			if (due < 1)
				due = 1;
			if (due < count)
				count = due;
		}

		OPL3_GenerateStream(&opl3, stream_adlib_buffer, count);
		SDL_PutAudioStreamData(stream, stream_adlib_buffer, count * 4);
		send += count;

		if (music_pit_divider)
		{
			music_pit_counter += count * 24;
			while (music_pit_counter >= 0)
			{
				music_pit_counter -= music_pit_divider;
				if (music_pit_callback)
					music_pit_callback();
			}
		}
	}

	return true;
}

static void Adlib_Init()
{
	OPL3_Reset(&opl3, 49716);
	SDL_AudioSpec spec;
	spec.format = SDL_AUDIO_S16;
	spec.channels = 2;
	spec.freq = 49716;
	stream_adlib = SDL_CreateAudioStream(&spec, &spec_main);
	if (!stream_adlib)
		return;

	SDL_SetAudioStreamGetCallback(stream_adlib, Adlib_Callback, NULL);

	SDL_BindAudioStream(device, stream_adlib);
}

void Adlib_Write(int offset, int data)
{
	if ((offset & 1) == 0)
	{
		opl3_address = data & 255;
		if ((offset & 2) != 0 && (opl3_address == 5 || opl3_newm))
			opl3_address |= 0x100;
	}
	else
	{
		if (opl3_address == 0x105)
			opl3_newm = data & 1;
		OPL3_WriteRegBuffered(&opl3, opl3_address, data);
	}
}

void Music_SetTimer(int divider, void (*handler)())
{
	music_pit_divider = divider;
	music_pit_callback = handler;
}

int Music_GetVolume()
{
	if (!stream_adlib)
		return 255;
	int vol = (int)(SDL_GetAudioStreamGain(stream_adlib) * 255.f * (1.f/ MUSIC_GAIN));

	if (vol < 0)
		vol = 0;
	else if (vol > 255)
		vol = 255;
	return vol;
}

void Music_SetVolume(int vol)
{
	if (!stream_adlib)
		return;
	SDL_SetAudioStreamGain(stream_adlib, vol * (MUSIC_GAIN / 255.f));
}

static bool Blaster_Callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
	if (stream != stream_blaster)
		return false;

	int bytespersample = blaster_stereo ? 2 : 1;
	bytespersample <<= blaster_16bit;

	int send = 0;

	while (send < total_amount)
	{
		int bytes = total_amount - send;
		if (bytes > BLASTER_BUFFER_SIZE)
			bytes = BLASTER_BUFFER_SIZE;

		if (blaster_pit_divider)
		{
			int due = (int)(-blaster_pit_counter * blaster_rate + 0.5);
			if (due < 1)
				due = 1;
			due *= bytespersample;
			if (due < bytes)
				bytes = due;
		}

		if (blaster_dma_running)
		{
			int dma_bytes = (blaster_dma_count + 1) << blaster_16bit;
			int irq_bytes = (blaster_irq_count + 1) << blaster_16bit;
			if (bytes > dma_bytes)
				bytes = dma_bytes;
			if (bytes > irq_bytes)
				bytes = irq_bytes;

			memcpy(stream_blaster_buffer, blaster_dma_buffer + (blaster_dma_ptr << blaster_16bit), bytes);
			blaster_dma_ptr += bytes >> blaster_16bit;
		}
		else
		{
			if (blaster_16bit)
				memset(stream_blaster_buffer, 0, bytes);
			else
				memset(stream_blaster_buffer, 0x80, bytes);
		}

		SDL_PutAudioStreamData(stream, stream_blaster_buffer, bytes);

		if (blaster_dma_running)
		{
			blaster_dma_count -= bytes >> blaster_16bit;
			if (blaster_dma_count < 0)
			{
				blaster_dma_count = blaster_dma_count_init;
				blaster_dma_ptr = blaster_dma_ptr_init;
			}
			blaster_irq_count -= bytes >> blaster_16bit;
			if (blaster_irq_count < 0)
			{
				blaster_irq_count = blaster_irq_count_init;
				if (!blaster_dma_auto)
					blaster_dma_running = 0;
				if (blaster_irq_callback)
					blaster_irq_callback();
			}
		}

		if (blaster_pit_divider)
		{
			blaster_pit_counter += (bytes / bytespersample) / (double)blaster_rate;
			while (blaster_pit_counter >= 0)
			{
				blaster_pit_counter -= blaster_pit_divider * (1.0/1193182.0);
				if (blaster_pit_callback)
					blaster_pit_callback();
			}
		}

		send += bytes;
	}

	return true;
}

void Blaster_Init(int tc, int rate, int channels, int is16bit)
{
	if (blaster_type != BLASTER_TYPE_16)
	{
		if (tc >= 65536)
			rate = 22050;
		else
			rate = 256000000 / ((65536 - tc) * channels);
	}

	blaster_rate = rate;
	blaster_16bit = is16bit;
	blaster_stereo = channels >= 2;

	blaster_dma_running = 0;

	if (stream_blaster)
	{
		SDL_UnbindAudioStream(stream_blaster);
		SDL_DestroyAudioStream(stream_blaster);
	}

	SDL_AudioSpec spec;
	spec.format = is16bit ? SDL_AUDIO_S16 : SDL_AUDIO_U8;
	spec.channels = channels;
	spec.freq = rate;

	stream_blaster = SDL_CreateAudioStream(&spec, &spec_main);
	if (!stream_blaster)
		return;

	SDL_SetAudioStreamGetCallback(stream_blaster, Blaster_Callback, NULL);
	SDL_BindAudioStream(device, stream_blaster);
}

char *Blaster_SetDmaPageSize(int size)
{
	if (size == 0 && blaster_dma_buffer2)
	{
		free(blaster_dma_buffer2);
		blaster_dma_buffer = NULL;
		blaster_dma_buffer2 = NULL;
		return NULL;
	}
	if (size == blaster_dma_buffer_size)
		return blaster_dma_buffer;

	int size2 = size + 15;
	blaster_dma_buffer2 = realloc(blaster_dma_buffer2, size2);

	blaster_dma_buffer_size = size;
	blaster_dma_buffer = (char*)(((intptr_t)blaster_dma_buffer2 + 15) & (~(intptr_t)15));

	return blaster_dma_buffer;
}

void Blaster_StartDma(int start, int dma_count, int count, int is_auto)
{
	blaster_dma_ptr = blaster_dma_ptr_init = start;
	blaster_dma_count = blaster_dma_count_init = dma_count;
	blaster_irq_count = blaster_irq_count_init = count;
	blaster_dma_auto = is_auto;
	blaster_dma_running = 1;
}

void Blaster_ContinueDma(int count)
{
	blaster_irq_count = blaster_irq_count_init = count;
	blaster_dma_running = 1;
}

void Blaster_StopDma()
{
	blaster_dma_running = 0;
}

void Blaster_Shutdown()
{
	if (!stream_blaster)
		return;

	SDL_UnbindAudioStream(stream_blaster);
	SDL_DestroyAudioStream(stream_blaster);
	stream_blaster = NULL;
}

void Blaster_SetIrqHandler(void (*handler)())
{
	blaster_irq_callback = handler;
}

int Blaster_GetVolume()
{
	if (!stream_blaster)
		return 255;
	int vol = (int)(SDL_GetAudioStreamGain(stream_blaster) * 255.f);

	if (vol < 0)
		vol = 0;
	else if (vol > 255)
		vol = 255;
	return vol;
}

void Blaster_SetVolume(int vol)
{
	if (!stream_blaster)
		return;
	SDL_SetAudioStreamGain(stream_blaster, vol * (1.f / 255.f));
}

int Blaster_GetDmaCount()
{
	return blaster_dma_count;
}

int Blaster_GetDmaPos()
{
	return blaster_dma_ptr;
}

void Sound_Init(int rate)
{
	if (sound_init)
		return;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0)
		goto failed;

	spec_main.format = SDL_AUDIO_S16;
	spec_main.channels = 2;
	spec_main.freq = rate;

	device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec_main);
	if (!device)
		goto failed;

	Adlib_Init();

	sound_rate = rate;
	sound_init = 1;

	snd_mutex = SDL_CreateMutex();

	return;
failed:
	printf("Sound_Init failed\n");
}

