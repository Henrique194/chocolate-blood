#include "compat.h"
#include "system.h"
#include "types.h"
#include "mouse.h"
#include "keyboard.h"
#include "joystick.h"

static int PauseCount;
static boolean ExtendedKeyFlag;

static byte PauseScanCode[6] = {
	0xe1, 0x1d, 0x45, 0xe1, 0x9d, 0xc5
};

static byte ExtendedScanCodes[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,sc_kpad_Enter,sc_RightControl,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,sc_kpad_Slash,0,sc_PrintScreen,sc_RightAlt,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,sc_Home,sc_UpArrow,sc_PgUp,0,sc_LeftArrow,0,sc_RightArrow,0,sc_End,
	sc_DownArrow,sc_PgDn,sc_Insert,sc_Delete,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

static boolean KEYBOARD_Started;

boolean MOUSE_Init(void)
{
	return true;
}

void MOUSE_Shutdown(void)
{
}

int32_t MOUSE_GetButtons()
{
	return Sys_GetMouseButtons();
}

void MOUSE_GetDelta(int32* x, int32* y)
{
	float dx, dy;
	Sys_GetMouseDelta(&dx, &dy);
	*x = dx; *y = dy;
}

void KEYBOARD_Isr()
{
	boolean f;
	byte b = kb_byte;
	byte h, l;
	f = true;
	if (PauseScanCode[PauseCount] == b)
	{
		PauseCount++;
		if (PauseCount == 6)
		{
			b = sc_Pause;
			PauseCount = 0;
		}
		else
			f = false;
	}
	if (f)
	{
		PauseCount = 0;
		if (KB_KeyPressed(sc_Pause))
			KB_ClearKeyDown(sc_Pause);

		if (b == 0xe0)
			ExtendedKeyFlag = true;
		else
		{
			h = b & 0x80;
			l = b & 0x7f;
			if (ExtendedKeyFlag)
			{
				if (l == sc_LeftShift || l == sc_RightShift)
					b = 0;
				else
					l = ExtendedScanCodes[l];
			}
			if (b && l)
				KB_KeyEvent(l, !h);
			ExtendedKeyFlag = false;
		}
	}
}

void KEYBOARD_Startup()
{
	if (KEYBOARD_Started)
		return;
	KEYBOARD_Started = true;
	PauseCount = 0;
	ExtendedKeyFlag = false;

	Sys_SetKeyboardHandler(KEYBOARD_Isr);
}

void KEYBOARD_Shutdown()
{
	if (!KEYBOARD_Started)
		return;
	Sys_SetKeyboardHandler(NULL);

	PauseCount = 0;
	ExtendedKeyFlag = false;

	KEYBOARD_Started = false;
}

int32 Joy_x1;
int32 Joy_y1;
int32 Joy_x2;
int32 Joy_y2;
byte Joy_mask;

void JoyStick_Vals(void)
{
	Joy_x1 = 0;
	Joy_y1 = 0;
	Joy_x2 = 0;
	Joy_y2 = 0;
}
