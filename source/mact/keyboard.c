#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compat.h"
#include "system.h"
#include "types.h"
#include "develop.h"
#include "keyboard.h"
#include "_keyboar.h"

static boolean KeyboardStarted;

static char  ASCIINames[] =          // Unshifted ASCII for scan codes
													 {
//       0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
		  0  ,27 ,'1','2','3','4','5','6','7','8','9','0','-','=',8  ,9  ,        // 0
		  'q','w','e','r','t','y','u','i','o','p','[',']',13 ,0  ,'a','s',        // 1
		  'd','f','g','h','j','k','l',';',39 ,'`',0  ,92 ,'z','x','c','v',        // 2
		  'b','n','m',',','.','/',0  ,'*',0  ,' ',0  ,0  ,0  ,0  ,0  ,0  ,        // 3
		  0  ,0  ,0  ,0  ,0  ,0  ,0  ,'7','8','9','-','4','5','6','+','1',        // 4
		  '2','3','0','.',0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,        // 5
		  0  ,0  ,0  ,0  ,0  ,0  ,0  ,'/',13 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,        // 6
		  0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0           // 7
													 },
		  ShiftNames[] =              // Shifted ASCII for scan codes
													 {
//       0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
		  0  ,27 ,'!','@','#','$','%','^','&','*','(',')','_','+',8  ,9  ,        // 0
		  'Q','W','E','R','T','Y','U','I','O','P','{','}',13 ,0  ,'A','S',        // 1
		  'D','F','G','H','J','K','L',':',34 ,'~',0  ,'|','Z','X','C','V',        // 2
		  'B','N','M','<','>','?',0  ,'*',0  ,' ',0  ,0  ,0  ,0  ,0  ,0  ,        // 3
		  0  ,0  ,0  ,0  ,0  ,0  ,0  ,'7','8','9','-','4','5','6','+','1',        // 4
		  '2','3','0','.',0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,        // 5
		  0  ,0  ,0  ,0  ,0  ,0  ,0  ,'/',13 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,        // 6
		  0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0           // 7
													 };

static char* ScanStrings[] = {
	"N/A", "Escape", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BakSpc", "Tab",
	"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "LCtrl", "A", "S",
	"D", "F", "G", "H", "J", "K", "L", ";", "'", "`", "LShift", "\\", "Z", "X", "C", "V",
	"B", "N", "M", ",", ".", "/", "RShift", "Kpad*", "LAlt", "Space", "CapLck", "F1" , "F2", "F3", "F4", "F5",
	"F6", "F7", "F8", "F9", "F10", "NumLck", "ScrLck", "KPad7", "KPad8", "Kpad9", "Kpad-", "Kpad4", "KPad5", "KPad6", "Kpad+", "Kpad1",
	"Kpad2", "Kpad3", "Kpad0", "Kpad.","N/A","N/A","N/A", "F11", "F12", "Pause", "Up", "N/A", "N/A", "N/A", "Insert", "Delete",
	"N/A", "Home", "End", "PgUp", "PgDn", "RAlt", "RCtrl", "Kpad/", "KpdEnt", "PrtScn", "Down", "Left", "Right",
	""
};

static int32 Keyhead, Keytail;

static char KeyboardQueue[KEYQMAX];

volatile byte KB_KeyDown[MAXKEYBOARDSCAN];

static boolean KeyPadActive;

volatile kb_scancode KB_LastScan;


void KB_KeyEvent(int scancode, boolean keypressed)
{
	char c;
	if (!keypressed)
	{
		KB_ClearKeyDown(scancode);
		return;
	}

	KB_SetKeyDown(scancode);
	KB_SetLastScanCode(scancode);

	if (!KB_KeypadActive() && IS_KEYPAD_CODE(scancode))
		c = 0;
	else if (KB_KeyPressed(sc_RightShift) || KB_KeyPressed(sc_LeftShift))
		c = ShiftNames[scancode];
	else
		c = ASCIINames[scancode];
	KB_Addch(c);
	if (c == 0)
		KB_Addch(scancode);
}

boolean KB_KeyWaiting(void)
{
	return Keyhead != Keytail;
}

char KB_Getch(void)
{
	char c;

	while (!KB_KeyWaiting()) {}

	c = KeyboardQueue[Keyhead];
	Keyhead = (Keyhead + 1) & (KEYQMAX - 1);

	return c;
}

void KB_Addch(char ch)
{
	int i;

	i = (Keytail + 1) & (KEYQMAX - 1);
	if (i != Keyhead)
	{
		KeyboardQueue[Keytail] = ch;
		Keytail = i;
	}
}

void KB_TurnKeypadOn(void)
{
	KeyPadActive = true;
}

void KB_TurnKeypadOff(void)
{
	KeyPadActive = false;
}

boolean KB_KeypadActive(void)
{
	return KeyPadActive;
}

void KB_FlushKeyboardQueue()
{
	Keyhead = 0;
	Keytail = 0;
}

void KB_ClearKeysDown(void)
{
	KB_LastScan = 0;
	memset(KB_KeyDown, 0, sizeof(KB_KeyDown));
}

char* KB_ScanCodeToString(kb_scancode scancode)
{
	if (scancode >= 110)
		scancode = 0;

	return ScanStrings[scancode];
}

kb_scancode KB_StringToScanCode(char* string)
{
	kb_scancode s;
	for (s = 0; s < 110; s++)
	{
		if (!strcmpi(ScanStrings[s], string))
			return s;
	}

	return 0;
}

void KEYBOARD_Startup();
void KEYBOARD_Shutdown();

void KB_Startup(void)
{
	if (KeyboardStarted)
		return;
	KeyboardStarted = true;
	KB_TurnKeypadOff();
	KB_FlushKeyboardQueue();
	KB_ClearKeysDown();
	KEYBOARD_Startup();
	printf("KB_Startup: Keyboard Started\n");
}

void KB_Shutdown(void)
{
	if (!KeyboardStarted)
		return;
	KeyboardStarted = false;
	KEYBOARD_Shutdown();
	KB_FlushKeyboardQueue();
	KB_ClearKeysDown();
}

