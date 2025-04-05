#include <SDL3/SDL.h>
#include <stdio.h>
#include "compat.h"
#include "video.h"


static int graphics_init = 0;
static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Texture* texture;

char video_palette[768];
char* video_buffer;
static char* video_buffer2;
static uint32_t* video_buffer_rgba;
int video_pages;
int video_row_stride;
int video_page_stride;
int video_xdim, video_ydim;
int video_graphics;

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

void Video_Set(int graphics, int w, int h)
{
	if (!graphics)
	{
		// text
		w = 640;
		h = 400;
	}

	int window_w = w, window_h = h;

	if (w < 640)
	{
		window_w = w << 1;
		window_h = h << 1;
	}

	if (!graphics_init)
	{
		CreateWindow(window_w, window_h);
		graphics_init = 1;
	}
	else
	{
		ResizeWindow(window_w, window_h);
	}

	video_graphics = graphics;

	if (video_buffer2)
	{
		free(video_buffer2);
		video_buffer2 = NULL;
		video_buffer = NULL;
	}

	if (graphics)
	{
		video_pages = 2;
		video_xdim = w;
		video_ydim = h;
		video_row_stride = w;
		video_page_stride = w * h;

		int buffer_size = w * h * video_pages + 15;
		video_buffer2 = malloc(buffer_size);
		video_buffer = (char*)((intptr_t)video_buffer2 & ~(intptr_t)15);

		video_buffer_rgba = malloc(w * h * 4);
	}
	else
	{
	}
}

void Video_BlitPage(int32_t page)
{
	if (!video_graphics)
		return;

	static uint32_t pal[256];

	for (int i = 0; i < 256; i++)
	{
		uint8_t r = video_palette[i * 3 + 0] & 63;
		uint8_t g = video_palette[i * 3 + 1] & 63;
		uint8_t b = video_palette[i * 3 + 2] & 63;

		r = (r << 2) | (r >> 4);
		g = (g << 2) | (g >> 4);
		b = (b << 2) | (b >> 4);

		pal[i] = (r<<0) | (g<<8) | (b<<16) | (0xff<<24);
	}
	
	char* ptr = video_buffer + page * video_page_stride;

	for (int i = 0; i < video_xdim * video_ydim; i++)
	{
		video_buffer_rgba[i] = pal[ptr[i]];
	}

	SDL_UpdateTexture(texture, NULL, video_buffer_rgba, video_xdim << 2);
	SDL_RenderTexture(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);
}


