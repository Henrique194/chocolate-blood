#include <SDL3/SDL.h>
#include <stdio.h>
#include "compat.h"
#include "video.h"


static int graphics_init = 0;
static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Texture* texture;
static SDL_Mutex* video_mutex;

char video_palette[768];
char* video_buffer;
static char* video_buffer2;
static uint32_t* video_buffer_rgba;
int video_pages;
int video_row_stride;
int video_page_stride;
int video_xdim, video_ydim;
int video_graphics;
char* video_text_buffer;
static int old_video_w, old_video_h;
int video_text_x, video_text_y;
int video_text_lines;
int video_text_cursor_active = 1;
int video_text_cnt;

static char textmode_palette[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x2a, 0x00, 0x00, 0x2a, 0x2a, 0x2a, 0x00, 0x00, 0x2a, 0x00, 0x2a, 0x2a, 0x15, 0x00, 0x2a, 0x2a, 0x2a,
	0x15, 0x15, 0x15, 0x15, 0x15, 0x3f, 0x15, 0x3f, 0x15, 0x15, 0x3f, 0x3f, 0x3f, 0x15, 0x15, 0x3f, 0x15, 0x3f, 0x3f, 0x3f, 0x00, 0x3f, 0x3f, 0x3f
};

extern const uint8_t g_vga_8x16TextFont[];

static void CreateWindow(int w, int h)
{
	if (window)
		return;

	if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
		goto failed;
	window = SDL_CreateWindow("Build Engine", w, h, 0);
	if (!window)
		goto failed;

	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer)
		goto failed;

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	if (!texture)
		goto failed;



	SDL_SetWindowRelativeMouseMode(window, true);

	return;
failed:
	printf("CreateWindow failed\n");
	exit(0);
}

static void ResizeWindow(int w, int h)
{
	SDL_SetWindowSize(window, w, h);

	SDL_DestroyTexture(texture);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	if (!texture)
	{
		printf("ResizeWindow failed\n");
		exit(0);
	}	
}

void Video_Init()
{
	video_text_lines = 25;
	video_text_buffer = malloc(80 * video_text_lines * 2);

	video_mutex = SDL_CreateMutex();
}

void Video_Set(int graphics, int w, int h)
{
	SDL_LockMutex(video_mutex);

	video_graphics = graphics;

	if (graphics == 0x13)
	{
		if (video_buffer2)
		{
			free(video_buffer2);
			video_buffer2 = NULL;
			video_buffer = NULL;
		}
		video_pages = 2;
		video_xdim = w;
		video_ydim = h;
		video_row_stride = w;
		video_page_stride = w * h;

		int buffer_size = w * h * video_pages + 15;
		video_buffer2 = malloc(buffer_size);
		video_buffer = (char*)((intptr_t)video_buffer2 & ~(intptr_t)15);

		free(video_buffer_rgba);
		video_buffer_rgba = malloc(w * h * 4);
	}
	else
	{
		memcpy(video_palette, textmode_palette, sizeof(textmode_palette));
		memset(video_text_buffer, 0, 80 * video_text_lines * 2);
		video_text_x = 0;
		video_text_y = 0;

		free(video_buffer_rgba);
		video_buffer_rgba = malloc(720 * 16 * video_text_lines * 4);
	}



	SDL_UnlockMutex(video_mutex);
}

static int active_page;

void Video_BlitPage(int32_t page)
{
	if (video_graphics != 0x13)
		return;

	active_page = page;
}

void Video_Blit()
{
	static uint32_t pal[256];

	SDL_LockMutex(video_mutex);

	int window_w, window_h;

	if (video_graphics == 0x13)
	{
		window_w = video_xdim;
		window_h = video_ydim;
	}
	else
	{
		window_w = 720;
		window_h = 400;
	}

	if (!graphics_init)
	{
		CreateWindow(window_w, window_h);
		graphics_init = 1;
	}
	else if (old_video_w != window_w || old_video_h != window_h)
	{
		old_video_w = window_w;
		old_video_h = window_h;
		ResizeWindow(window_w, window_h);
	}

	for (int i = 0; i < 256; i++)
	{
		uint8_t r = video_palette[i * 3 + 0] & 63;
		uint8_t g = video_palette[i * 3 + 1] & 63;
		uint8_t b = video_palette[i * 3 + 2] & 63;

		r = (r << 2) | (r >> 4);
		g = (g << 2) | (g >> 4);
		b = (b << 2) | (b >> 4);

		pal[i] = (r << 0) | (g << 8) | (b << 16) | (0xff << 24);
	}

	if (video_graphics == 0x13)
	{

		char* ptr = video_buffer + active_page * video_page_stride;

		for (int i = 0; i < video_xdim * video_ydim; i++)
		{
			video_buffer_rgba[i] = pal[ptr[i]];
		}

		SDL_UpdateTexture(texture, NULL, video_buffer_rgba, video_xdim << 2);
	}
	else
	{
		int stride = 16 * 720;
		for (int i = 0; i < video_text_lines; i++)
		{
			uint32_t *dest = video_buffer_rgba + i * stride;
			char* src = video_text_buffer + i * 160;
			for (int j = 0; j < 80; j++)
			{
				char c = src[j * 2];
				char a = src[j * 2 + 1];
				int fc = a & 15;
				int bc = (a >> 4) & 7;
				int bl = (a & 128) != 0 && (video_text_cnt & 32) != 0;
				uint8_t* fnt = &g_vga_8x16TextFont[c * 8 * 16];
				uint32_t *dest2 = dest + 9 * j;

				int is_cursor = (video_text_cnt & 8) != 0 && video_text_cursor_active
					&& video_text_x == j && video_text_y == i;


				for (int y = 0; y < 16; y++)
				{
					for (int x = 0; x < 9; x++)
					{
						int col;
						if (is_cursor && (y == 13 || y == 14))
						{
							col = fc;
						}
						else if (bl)
						{
							col = bc;
						}
						else if (x == 8 && !(c >= 192 && c < 224))
						{
							col = bc;
						}
						else
						{
							int x2 = x;
							if (x2 >= 8)
								x2 = 7;
							col = fnt[x2] ? fc : bc;
						}
						dest2[x] = pal[col];
					}
					dest2 += 720;
					fnt += 8;
				}
			}
		}

		SDL_UpdateTexture(texture, NULL, video_buffer_rgba, 720 << 2);

		video_text_cnt++;
	}
	SDL_RenderTexture(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);

	SDL_UnlockMutex(video_mutex);
}

void Video_Text_Puts(const char* s)
{
	while (*s)
	{
		switch (*s)
		{
			default:
				video_text_buffer[video_text_x * 2 + video_text_y * 160] = *s;
				video_text_buffer[video_text_x * 2 + video_text_y * 160 + 1] = 0x07;
				video_text_x++;
				if (video_text_x < 80)
					break;
				__fallthrough;
			case '\n':
				video_text_y++;
				video_text_x = 0;
				if (video_text_y >= video_text_lines)
				{
					memmove(video_text_buffer, video_text_buffer + 80 * 2, (video_text_lines - 1) * 80 * 2);
					memset(video_text_buffer + (video_text_lines - 1) * 80 * 2, 0, 80 * 2);
					video_text_y = video_text_lines - 1;
				}
				break;
			case '\r':
				video_text_x = 0;
				break;
		}
		s++;
	}
}

void Video_Text_SetCursor(int x, int y)
{
	video_text_x = x;
	video_text_y = y;
}

void Video_Text_Scroll(int lines, int attr, int x1, int y1, int x2, int y2)
{
	if (lines == 0)
	{
		for (int y = y1; y <= y2; y++)
		{
			for (int x = x1; x <= x2; x++)
			{
				video_text_buffer[y * 160 + x * 2] = '\0';
				video_text_buffer[y * 160 + x * 2 + 1] = attr;
			}
		}
	}
	else
	{
		for (int y = y1; y <= y2; y++)
		{
			int cy = y + lines;
			if (cy > y2)
			{
				for (int x = x1; x <= x2; x++)
				{
					video_text_buffer[y * 160 + x * 2] = '\0';
					video_text_buffer[y * 160 + x * 2 + 1] = attr;
				}
			}
			else
			{
				memcpy(video_text_buffer + y * 160 + x1 * 2, video_text_buffer + cy * 160 + x1 * 2,
					(x2 - x1 + 1) * 2);
			}
		}
		video_text_y -= lines;
		if (video_text_y < y1)
			video_text_y = y1;
	}
}
