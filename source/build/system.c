#include <SDL3/SDL.h>
#include "compat.h"
#include "system.h"

static uint64_t timer_base;
static uint64_t pit_cycles;
static int pit_divider;
static void (*pit_callback)();
static void (*kb_callback)();
char kb_byte;
#define KB_EXTENDED_BIT 0x80

static int kb_scancodemap[SDL_SCANCODE_COUNT];

static void PIT_Update()
{
	uint64_t t = SDL_GetPerformanceCounter() - timer_base;
	uint64_t cycles = (t * 1193182) / SDL_GetPerformanceFrequency();

	if (pit_divider && pit_callback)
	{
		while (pit_cycles < cycles)
		{
			pit_callback();
			pit_cycles += pit_divider;
		}
	}
	else
		pit_cycles = cycles;
}

void Sys_Init()
{
	SDL_InitSubSystem(SDL_INIT_EVENTS);

	timer_base = SDL_GetPerformanceCounter();

	memset(kb_scancodemap, -1, sizeof(kb_scancodemap));

	kb_scancodemap[SDL_SCANCODE_ESCAPE] = 1;
	kb_scancodemap[SDL_SCANCODE_1] = 2;
	kb_scancodemap[SDL_SCANCODE_2] = 3;
	kb_scancodemap[SDL_SCANCODE_3] = 4;
	kb_scancodemap[SDL_SCANCODE_4] = 5;
	kb_scancodemap[SDL_SCANCODE_5] = 6;
	kb_scancodemap[SDL_SCANCODE_6] = 7;
	kb_scancodemap[SDL_SCANCODE_7] = 8;
	kb_scancodemap[SDL_SCANCODE_8] = 9;
	kb_scancodemap[SDL_SCANCODE_9] = 10;
	kb_scancodemap[SDL_SCANCODE_0] = 11;
	kb_scancodemap[SDL_SCANCODE_MINUS] = 12;
	kb_scancodemap[SDL_SCANCODE_EQUALS] = 13;
	kb_scancodemap[SDL_SCANCODE_BACKSPACE] = 14;
	kb_scancodemap[SDL_SCANCODE_TAB] = 15;
	kb_scancodemap[SDL_SCANCODE_Q] = 16;
	kb_scancodemap[SDL_SCANCODE_W] = 17;
	kb_scancodemap[SDL_SCANCODE_E] = 18;
	kb_scancodemap[SDL_SCANCODE_R] = 19;
	kb_scancodemap[SDL_SCANCODE_T] = 20;
	kb_scancodemap[SDL_SCANCODE_Y] = 21;
	kb_scancodemap[SDL_SCANCODE_U] = 22;
	kb_scancodemap[SDL_SCANCODE_I] = 23;
	kb_scancodemap[SDL_SCANCODE_O] = 24;
	kb_scancodemap[SDL_SCANCODE_P] = 25;
	kb_scancodemap[SDL_SCANCODE_LEFTBRACKET] = 26;
	kb_scancodemap[SDL_SCANCODE_RIGHTBRACKET] = 27;
	kb_scancodemap[SDL_SCANCODE_RETURN] = 28;
	kb_scancodemap[SDL_SCANCODE_LCTRL] = 29;
	kb_scancodemap[SDL_SCANCODE_A] = 30;
	kb_scancodemap[SDL_SCANCODE_S] = 31;
	kb_scancodemap[SDL_SCANCODE_D] = 32;
	kb_scancodemap[SDL_SCANCODE_F] = 33;
	kb_scancodemap[SDL_SCANCODE_G] = 34;
	kb_scancodemap[SDL_SCANCODE_H] = 35;
	kb_scancodemap[SDL_SCANCODE_J] = 36;
	kb_scancodemap[SDL_SCANCODE_K] = 37;
	kb_scancodemap[SDL_SCANCODE_L] = 38;
	kb_scancodemap[SDL_SCANCODE_SEMICOLON] = 39;
	kb_scancodemap[SDL_SCANCODE_APOSTROPHE] = 40;
	kb_scancodemap[SDL_SCANCODE_GRAVE] = 41;
	kb_scancodemap[SDL_SCANCODE_LSHIFT] = 42;
	kb_scancodemap[SDL_SCANCODE_BACKSLASH] = 43;
	kb_scancodemap[SDL_SCANCODE_Z] = 44;
	kb_scancodemap[SDL_SCANCODE_X] = 45;
	kb_scancodemap[SDL_SCANCODE_C] = 46;
	kb_scancodemap[SDL_SCANCODE_V] = 47;
	kb_scancodemap[SDL_SCANCODE_B] = 48;
	kb_scancodemap[SDL_SCANCODE_N] = 49;
	kb_scancodemap[SDL_SCANCODE_M] = 50;
	kb_scancodemap[SDL_SCANCODE_COMMA] = 51;
	kb_scancodemap[SDL_SCANCODE_PERIOD] = 52;
	kb_scancodemap[SDL_SCANCODE_SLASH] = 53;
	kb_scancodemap[SDL_SCANCODE_RSHIFT] = 54;
	kb_scancodemap[SDL_SCANCODE_KP_MULTIPLY] = 55;
	kb_scancodemap[SDL_SCANCODE_LALT] = 56;
	kb_scancodemap[SDL_SCANCODE_SPACE] = 57;
	kb_scancodemap[SDL_SCANCODE_CAPSLOCK] = 58;
	kb_scancodemap[SDL_SCANCODE_F1] = 59;
	kb_scancodemap[SDL_SCANCODE_F2] = 60;
	kb_scancodemap[SDL_SCANCODE_F3] = 61;
	kb_scancodemap[SDL_SCANCODE_F4] = 62;
	kb_scancodemap[SDL_SCANCODE_F5] = 63;
	kb_scancodemap[SDL_SCANCODE_F6] = 64;
	kb_scancodemap[SDL_SCANCODE_F7] = 65;
	kb_scancodemap[SDL_SCANCODE_F8] = 66;
	kb_scancodemap[SDL_SCANCODE_F9] = 67;
	kb_scancodemap[SDL_SCANCODE_F10] = 68;
	kb_scancodemap[SDL_SCANCODE_NUMLOCKCLEAR] = 69;
	kb_scancodemap[SDL_SCANCODE_SCROLLLOCK] = 70;
	kb_scancodemap[SDL_SCANCODE_KP_7] = 71;
	kb_scancodemap[SDL_SCANCODE_KP_8] = 72;
	kb_scancodemap[SDL_SCANCODE_KP_9] = 73;
	kb_scancodemap[SDL_SCANCODE_KP_MINUS] = 74;
	kb_scancodemap[SDL_SCANCODE_KP_4] = 75;
	kb_scancodemap[SDL_SCANCODE_KP_5] = 76;
	kb_scancodemap[SDL_SCANCODE_KP_6] = 77;
	kb_scancodemap[SDL_SCANCODE_KP_PLUS] = 78;
	kb_scancodemap[SDL_SCANCODE_KP_1] = 79;
	kb_scancodemap[SDL_SCANCODE_KP_2] = 80;
	kb_scancodemap[SDL_SCANCODE_KP_3] = 81;
	kb_scancodemap[SDL_SCANCODE_KP_0] = 82;
	kb_scancodemap[SDL_SCANCODE_KP_COMMA] = 83;
	kb_scancodemap[SDL_SCANCODE_F11] = 87;
	kb_scancodemap[SDL_SCANCODE_F12] = 88;

	kb_scancodemap[SDL_SCANCODE_KP_ENTER] = KB_EXTENDED_BIT | 28;
	kb_scancodemap[SDL_SCANCODE_RCTRL] = KB_EXTENDED_BIT | 29;
	kb_scancodemap[SDL_SCANCODE_KP_DIVIDE] = KB_EXTENDED_BIT | 53;
	kb_scancodemap[SDL_SCANCODE_RALT] = KB_EXTENDED_BIT | 56;
	kb_scancodemap[SDL_SCANCODE_HOME] = KB_EXTENDED_BIT | 71;
	kb_scancodemap[SDL_SCANCODE_UP] = KB_EXTENDED_BIT | 72;
	kb_scancodemap[SDL_SCANCODE_PAGEUP] = KB_EXTENDED_BIT | 73;
	kb_scancodemap[SDL_SCANCODE_LEFT] = KB_EXTENDED_BIT | 75;
	kb_scancodemap[SDL_SCANCODE_RIGHT] = KB_EXTENDED_BIT | 77;
	kb_scancodemap[SDL_SCANCODE_END] = KB_EXTENDED_BIT | 79;
	kb_scancodemap[SDL_SCANCODE_DOWN] = KB_EXTENDED_BIT | 80;
	kb_scancodemap[SDL_SCANCODE_PAGEDOWN] = KB_EXTENDED_BIT | 81;
	kb_scancodemap[SDL_SCANCODE_INSERT] = KB_EXTENDED_BIT | 82;
	kb_scancodemap[SDL_SCANCODE_DELETE] = KB_EXTENDED_BIT | 83;
	kb_scancodemap[SDL_SCANCODE_APPLICATION] = KB_EXTENDED_BIT | 94;

	pit_divider = 0;
	pit_callback = NULL;

	kb_callback = NULL;
}

void Sys_HandleEvents()
{
	SDL_PumpEvents();

	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
			case SDL_EVENT_KEY_DOWN:
			case SDL_EVENT_KEY_UP:
			{
				if (!kb_callback)
					break;
				if (ev.key.scancode == SDL_SCANCODE_PRINTSCREEN)
				{
					if (ev.type == SDL_EVENT_KEY_DOWN)
					{
						kb_byte = 0xe0; kb_callback();
						kb_byte = 0x2a; kb_callback();
						kb_byte = 0xe0; kb_callback();
						kb_byte = 0x37; kb_callback();
					}
					else
					{
						kb_byte = 0xe0; kb_callback();
						kb_byte = 0xb7; kb_callback();
						kb_byte = 0xe0; kb_callback();
						kb_byte = 0xaa; kb_callback();
					}
				}
				else
				{
					int mapped = kb_scancodemap[ev.key.scancode];
					if (mapped == -1)
						break;
					if (mapped & KB_EXTENDED_BIT)
					{
						kb_byte = 0xe0; kb_callback();
					}
					kb_byte = mapped & 0x7f;
					if (ev.type == SDL_EVENT_KEY_UP)
						kb_byte |= 0x80;
					kb_callback();
				}

				break;
			}
		}
	}

	PIT_Update();
}

void Sys_SetTimer(int divider, void (*handler)())
{
	pit_divider = divider;
	pit_callback = handler;
}

void Sys_SetKeyboardHandler(void (*handler)())
{
	kb_callback = handler;
}