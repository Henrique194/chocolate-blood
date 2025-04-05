#pragma once
#include <SDL3/SDL.h>

extern SDL_Mutex* snd_mutex;

typedef enum
{
	BLASTER_TYPE_NONE = 0,
	BLASTER_TYPE_1,
	BLASTER_TYPE_2,
	BLASTER_TYPE_PRO,
	BLASTER_TYPE_16,
} BlasterType_t;

extern int blaster_type;
void Sound_Init(int rate);
void Blaster_SetIrqHandler(void (*handler)());
char *Blaster_SetDmaPageSize(int size);
void Blaster_Init(int tc, int rate, int channels, int is16bit);
void Blaster_StartDma(int start, int dma_count, int count, int is_auto);
void Blaster_ContinueDma(int count);
void Blaster_StopDma();
void Blaster_Shutdown();
int Blaster_GetDmaCount();
int Blaster_GetDmaPos();
int Blaster_GetVolume();
void Blaster_SetVolume(int vol);
int Music_GetVolume();
void Music_SetVolume(int vol);
void Music_SetTimer(int divider, void (*handler)());
void Adlib_Write(int offset, int data);
