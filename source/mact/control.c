#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compat.h"
#include "system.h"
#include "types.h"
#include "develop.h"
#include "keyboard.h"
#include "util_lib.h"
#include "control.h"
#include "_control.h"
#include "mouse.h"
#include "external.h"
#include "joystick.h"
#include "fixed.h"

boolean CONTROL_RudderEnabled = false;
boolean CONTROL_MouseEnabled = false;
boolean CONTROL_ExternalEnabled = false;
boolean CONTROL_JoystickEnabled = false;
byte CONTROL_JoystickPort = 0;
static int32 CONTROL_UserInputDelay = -1;
static int32 CONTROL_MouseSensitivity = DEFAULTMOUSESENSITIVITY;
static int32 CONTROL_JoyXAxis = 0;
static int32 CONTROL_JoyYAxis = 0;
static int32 CONTROL_JoyXAxis2 = 0;
static int32 CONTROL_JoyYAxis2 = 0;
static int32 CONTROL_JoyHatBias = 0;
static boolean CONTROL_HatEnabled = false;
static boolean CONTROL_ThrottleEnabled = false;
static boolean CONTROL_FlightStickMode = false;
static int32 CONTROL_NumButtons = MAXBUTTONS;
static int32 CONTROL_NumAxes = MAXAXES;

static JoystickDef JoyDefs[2];
static ExternalControlInfo *CONTROL_External;

static int32 CONTROL_DoubleClickSpeed;

static controlaxismaptype CONTROL_AxesMap[MAXAXES];
static controlaxistype CONTROL_Axes[MAXAXES];
static controlaxistype CONTROL_LastAxes[MAXAXES];
static int32 CONTROL_AxesScale[MAXAXES];
static controlflags CONTROL_Flags[CONTROL_NUM_FLAGS];
static controlkeymaptype CONTROL_KeyMapping[CONTROL_NUM_FLAGS];
static controlbuttontype CONTROL_DeviceButtonMapping[CONTROL_NUM_FLAGS];
static int32 CONTROL_DeviceButtonState[MAXBUTTONS];
static int32 CONTROL_ButtonClickedTime[MAXBUTTONS];
static int32 CONTROL_ButtonClickedState[MAXBUTTONS];
static int32 CONTROL_ButtonClicked[MAXBUTTONS];
static byte CONTROL_ButtonClickedCount[MAXBUTTONS];

static int32 ticrate;

static int32 (*GetTime)();

static int32 CONTROL_UserInputCleared[3];

uint32 CONTROL_ButtonHeldState1;
uint32 CONTROL_ButtonHeldState2;
boolean CONTROL_MousePresent;
uint32 CONTROL_ButtonState1;
uint32 CONTROL_ButtonState2;

static boolean CONTROL_Started;

void CONTROL_GetMouseDelta(int32* x, int32* y)
{
    MOUSE_GetDelta(x, y);
    if (abs(*x) > abs(*y))
    {
        *y /= 3;
    }
    else
    {
        *x /= 3;
    }

    *x *= 32;
    *y *= 96;

    *x = FixedMulShift(*x, CONTROL_MouseSensitivity, 15);
}

int32 CONTROL_GetMouseSensitivity(void)
{
    return CONTROL_MouseSensitivity - MINIMUMMOUSESENSITIVITY;
}

void CONTROL_SetMouseSensitivity(int32 newsensitivity)
{
    CONTROL_MouseSensitivity = newsensitivity + MINIMUMMOUSESENSITIVITY;
}

byte CONTROL_GetMouseButtons(void)
{
    return MOUSE_GetButtons();
}

boolean CONTROL_StartMouse(void)
{
    CONTROL_NumButtons = MAXMOUSEBUTTONS;
    return MOUSE_Init();
}

void CONTROL_GetJoyAbs(int32 joy)
{
    int x, y;
    if (joy)
    {
        Joy_mask = 12;
        JOYSTICK_GetPos(&x, &y, &CONTROL_JoyXAxis, &CONTROL_JoyYAxis);
    }
    else
    {
        Joy_mask = 3;
        Joy_mask = CONTROL_RudderEnabled << 2;
        Joy_mask = (CONTROL_ThrottleEnabled | CONTROL_HatEnabled) << 3;
        JoyStick_Vals();
        JOYSTICK_GetPos(&CONTROL_JoyXAxis, &CONTROL_JoyYAxis, &CONTROL_JoyXAxis2, &CONTROL_JoyYAxis2);
    }
}

void CONTROL_FilterJoyDelta(int32 joy, int32 *dx, int32 *dy)
{
    JoystickDef* def = &JoyDefs[joy];
    int x = *dx;
    int y = *dy;

    if (x < def->threshMinX)
    {
        if (x < def->joyMinX)
            x = def->joyMinX;

        x = -(x - def->threshMinX);
        x *= def->joyMultXL;
        *dx = -x;
    }
    else if (x > def->threshMaxX)
    {
        if (x > def->joyMaxX)
            x = def->joyMaxX;

        x = x - def->threshMaxX;
        x *= def->joyMultXH;
        *dx = x;
    }
    else
        *dx = 0;

    if (y < def->threshMinY)
    {
        if (y < def->joyMinY)
            y = def->joyMinY;

        y = -(y - def->threshMinY);
        y *= def->joyMultYL;
        *dy = -y;
    }
    else if (y > def->threshMaxY)
    {
        if (y > def->joyMaxY)
            y = def->joyMaxY;

        y = y - def->threshMaxY;
        y *= def->joyMultYH;
        *dy = y;
    }
    else
        *dy = 0;
}

void CONTROL_GetJoyDelta
(
    int32 joy,
    int32* dx,
    int32* dy,
    int32* rudder,
    int32* throttle
)
{
    CONTROL_GetJoyAbs(joy);
    *dx = CONTROL_JoyXAxis;
    *dy = CONTROL_JoyYAxis;
    CONTROL_FilterJoyDelta(joy, dx, dy);
    if (joy == 0 && (CONTROL_RudderEnabled || CONTROL_ThrottleEnabled))
    {
        *rudder = CONTROL_JoyXAxis2;
        *throttle = CONTROL_JoyYAxis2;
        CONTROL_FilterJoyDelta(1, rudder, throttle);
    }
}

byte CONTROL_JoyButtons(int32 joy)
{
    byte i, j;

    // TODO: i = inp(0x201);
    i = 255;
    if (joy)
        i >>= 6;
    else
        i >>= 4;
    if (joy == 0)
        j = 15;
    else
        j = 3;
    i &= j;
    i ^= j;

    if (CONTROL_FlightStickMode)
    {
        switch (i)
        {
            case 3:
                i = 128;
                break;
            case 7:
                i = 64;
                break;
            case 11:
                i = 32;
                break;
            case 15:
                i = 16;
                break;
        }
    }
    else if (CONTROL_HatEnabled)
    {
        switch ((CONTROL_JoyYAxis2 + CONTROL_JoyHatBias) / 100)
        {
            case 0:
                i |= 16;
                break;
            case 1:
                i |= 32;
                break;
            case 2:
                i |= 64;
                break;
            case 3:
                i |= 128;
                break;
        }
    }

    return i;
}

void CONTROL_SetJoyScale(int32 joy)
{
    int dx;
    int joymax = JoyMax;
    JoystickDef* def = &JoyDefs[joy];

    dx = def->threshMinX - def->joyMinX;
    if (dx == 0)
        dx = 1;
    def->joyMultXL = joymax / dx;
    dx = def->joyMaxX - def->threshMaxX;
    if (dx == 0)
        dx = 1;
    def->joyMultXH = joymax / dx;
    dx = def->threshMinY - def->joyMinY;
    if (dx == 0)
        dx = 1;
    def->joyMultYL = joymax / dx;
    dx = def->joyMaxY - def->threshMaxY;
    if (dx == 0)
        dx = 1;
    def->joyMultYH = joymax / dx;
}

void CONTROL_SetupJoy
(
    int32 joy, int32 minx,
    int32 maxx, int32 miny,
    int32 maxy, int32 centerx,
    int32 centery
)
{
    JoystickDef* def = &JoyDefs[joy];
    int dx;

    def->joyMinX = minx;
    def->joyMaxX = maxx;

    dx = (maxx - minx) / 12;
    if (dx < 1) dx = 1;
    def->threshMinX = centerx - dx;
    def->threshMaxX = centerx + dx;

    def->joyMinY = miny;
    def->joyMaxY = maxy;

    dx = (maxy - miny) / 12;
    if (dx < 1) dx = 1;
    def->threshMinY = centery - dx;
    def->threshMaxY = centery + dx;

    CONTROL_SetJoyScale(joy);
}

void CONTROL_CenterJoystick
(
    void (*CenterCenter)(void),
    void (*UpperLeft)(void),
    void (*LowerRight)(void),
    void (*CenterThrottle)(void),
    void (*CenterRudder)(void)
)
{
    UserInput info;
    int centerx, centery;
    int minx, miny;
    int maxx, maxy;
    int ct = 0, cr = 0;
    CONTROL_GetUserInput(&info);
    CONTROL_ClearUserInput(&info);
    CenterCenter();
    memset(&info, 0, sizeof(info));

    while (!info.button0 && !info.button1)
    {
        CONTROL_GetUserInput(&info);
    }
    CONTROL_GetJoyAbs(CONTROL_JoystickPort);

    centerx = CONTROL_JoyXAxis;
    centery = CONTROL_JoyYAxis;

    CONTROL_ClearUserInput(&info);
    UpperLeft();
    memset(&info, 0, sizeof(info));

    while (!info.button0 && !info.button1)
    {
        CONTROL_GetUserInput(&info);
    }
    CONTROL_GetJoyAbs(CONTROL_JoystickPort);

    minx = CONTROL_JoyXAxis;
    miny = CONTROL_JoyYAxis;

    CONTROL_ClearUserInput(&info);
    LowerRight();
    memset(&info, 0, sizeof(info));

    while (!info.button0 && !info.button1)
    {
        CONTROL_GetUserInput(&info);
    }
    CONTROL_GetJoyAbs(CONTROL_JoystickPort);

    maxx = CONTROL_JoyXAxis;
    maxy = CONTROL_JoyYAxis;

    CONTROL_ClearUserInput(&info);

    if (abs(minx - maxx) < centerx / 4)
    {
        minx = 0;
        maxx = centerx * 2;
    }
    if (abs(miny - maxy) < centery / 4)
    {
        miny = 0;
        maxy = centery * 2;
    }

    if (CONTROL_ThrottleEnabled)
    {
        CenterThrottle();
        memset(&info, 0, sizeof(info));

        while (!info.button0 && !info.button1)
        {
            CONTROL_GetUserInput(&info);
        }
        CONTROL_GetJoyAbs(CONTROL_JoystickPort);

        ct = CONTROL_JoyYAxis2;

        CONTROL_ClearUserInput(&info);
    }
    if (CONTROL_RudderEnabled)
    {
        CenterRudder();
        memset(&info, 0, sizeof(info));

        while (!info.button0 && !info.button1)
        {
            CONTROL_GetUserInput(&info);
        }
        CONTROL_GetJoyAbs(CONTROL_JoystickPort);

        cr = CONTROL_JoyXAxis2;

        CONTROL_ClearUserInput(&info);
    }

    CONTROL_SetupJoy(CONTROL_JoystickPort, minx, maxx, miny, maxy, centerx, centery);

    centerx = cr;
    centery = ct;
    minx = 0;
    maxx = centerx * 2;
    minx = 0;
    maxx = centery * 2;

    if (CONTROL_RudderEnabled || CONTROL_FlightStickMode)
    {
        CONTROL_SetupJoy(1, minx, maxx, miny, maxy, centerx, centery);
    }
}

void CONTROL_SetJoystickHatBias
(
    int32 newbias
)
{
    CONTROL_JoyHatBias = newbias;
}

int32 CONTROL_GetJoystickHatBias
(
    void
)
{
    return CONTROL_JoyHatBias;
}

boolean CONTROL_StartJoy(int32 joy)
{
    int x, y;
    CONTROL_GetJoyAbs(joy);

    x = CONTROL_JoyXAxis;
    y = CONTROL_JoyYAxis;

    if (x == 0 || x > 4990)
        return false;
    if (y == 0 || y > 4990)
        return false;

    CONTROL_SetupJoy(joy, 0, x * 2, 0, y * 2, x, y);
    CONTROL_NumButtons = MAXJOYBUTTONS;
    return true;
}

void CONTROL_ExternalStartup(void)
{
#if 0
    int parm;
    ExternalControlInfo* ext;
    if (CONTROL_ExternalEnabled)
        return;

    parm = CheckParm("control");
    if (!parm)
    {
        printf("CONTROL_Startup: %s parameter is not present on the command line\n", "control");
        return;
    }
    parm++;
    ext = (ExternalControlInfo*)atol(sys_argv[parm]);

    CONTROL_External = ext;
    if (CONTROL_External->id != CONTROLID)
    {
        printf("CONTROL_Startup: External API is incompatible with this version. id=%ld\n", CONTROL_External->id);
        return;
    }
    printf("CONTROL_Startup: External controller found on vector %x\n", CONTROL_External->intnum);
    CONTROL_ExternalEnabled = true;
#endif
}

void CONTROL_ExternalSetup(void)
{
    int i;
    int b;
    int a;
    CONTROL_NumButtons = MAXEXTERNALBUTTONS;
    for (i = 0; i < CONTROL_NumButtons; i++)
    {
        b = CONTROL_External->buttonmap[i][0];
        if (b != EXTERNALBUTTONUNDEFINED)
            CONTROL_MapButton(b, i, 0);
        b = CONTROL_External->buttonmap[i][1];
        if (b != EXTERNALBUTTONUNDEFINED)
            CONTROL_MapButton(b, i, 1);
    }
    for (i = 0; i < CONTROL_NumAxes; i++)
    {
        a = CONTROL_External->analogaxesmap[i];
        if (a != EXTERNALAXISUNDEFINED)
            CONTROL_MapAnalogAxis(i, a);
        b = CONTROL_External->digitalaxesmap[i][0];
        if (b != EXTERNALBUTTONUNDEFINED)
            CONTROL_MapDigitalAxis(i, b, 0);
        b = CONTROL_External->digitalaxesmap[i][1];
        if (b != EXTERNALBUTTONUNDEFINED)
            CONTROL_MapDigitalAxis(i, b, 1);
    }
}

void CONTROL_FillExternalInfo(ControlInfo *info)
{
    int i;
    for (i = 0; i < CONTROL_NumAxes; i++)
    {
        switch (CONTROL_AxesMap[i].analogmap)
        {
            case analog_turning:
                CONTROL_External->axes[i] = info->dyaw;
                break;
            case analog_strafing:
                CONTROL_External->axes[i] = info->dx;
                break;
            case analog_lookingupanddown:
                CONTROL_External->axes[i] = info->dpitch;
                break;
            case analog_elevation:
                CONTROL_External->axes[i] = info->dy;
                break;
            case analog_rolling:
                CONTROL_External->axes[i] = info->droll;
                break;
            case analog_moving:
                CONTROL_External->axes[i] = info->dz;
                break;
        }
    }
}

void CONTROL_PollExternalControl(void)
{
#if 0
    union REGS regs;
    int i;
    if (!CONTROL_ExternalEnabled)
        return;

    CONTROL_External->command = EXTERNAL_GetInput;
    int386(CONTROL_External->intnum, &regs, &regs);
    for (i = 0; i < CONTROL_NumAxes; i++)
    {
        CONTROL_Axes[i].analog = CONTROL_External->axes[i];
    }
#endif
}

int32 CONTROL_GetTime(void)
{
    static int tics = 0;

    tics += 5;
    return tics;
}

void CONTROL_CheckRange(int32 which)
{
    if (which < 0 || which >= CONTROL_NUM_FLAGS)
        Error("CONTROL_CheckRange : Index %d out of valid range for %d control flags.", which, CONTROL_NUM_FLAGS);
}

void CONTROL_SetFlag(int32 which, boolean active)
{
    CONTROL_CheckRange(which);
    if (!CONTROL_Flags[which].toggle)
    {
        CONTROL_Flags[which].active = active;
        return;
    }

    if (active)
    {
        if (!CONTROL_Flags[which].buttonheld)
        {
            CONTROL_Flags[which].active = !CONTROL_Flags[which].active;
            CONTROL_Flags[which].buttonheld = 1;
        }
    }
    else
        CONTROL_Flags[which].buttonheld = 0;
}

boolean CONTROL_KeyboardFunctionPressed(int32 whichfunction)
{
    boolean pressed = false;
    CONTROL_CheckRange(whichfunction);

    if (CONTROL_Flags[whichfunction].used)
    {
        pressed = KB_KeyPressed(CONTROL_KeyMapping[whichfunction].key1);
        pressed |= KB_KeyPressed(CONTROL_KeyMapping[whichfunction].key2);
    }

    return pressed;
}

void CONTROL_ClearKeyboardFunction(int32 whichfunction)
{
    CONTROL_CheckRange(whichfunction);

    if (CONTROL_Flags[whichfunction].used)
    {
        KB_ClearKeyDown(CONTROL_KeyMapping[whichfunction].key1);
        KB_ClearKeyDown(CONTROL_KeyMapping[whichfunction].key2);
    }
}

void CONTROL_DefineFlag(int32 which, boolean toggle)
{
    CONTROL_CheckRange(which);

    CONTROL_Flags[which].toggle = toggle;
    CONTROL_Flags[which].used = 1;
    CONTROL_Flags[which].active = 0;
    CONTROL_Flags[which].buttonheld = 0;
    CONTROL_Flags[which].cleared = 0;
    if (!CONTROL_FlagActive(which))
        Error("CONTROL_DefineFlag: assertion failed flag = %ld\n", which);
}

boolean CONTROL_FlagActive(int32 which)
{
    CONTROL_CheckRange(which);

    return CONTROL_Flags[which].used;
}

void CONTROL_MapKey(int32 which, kb_scancode key1, kb_scancode key2)
{
    CONTROL_CheckRange(which);

    CONTROL_KeyMapping[which].key1 = key1;
    CONTROL_KeyMapping[which].key2 = key2;
}

void CONTROL_GetKeyMap(int32 which, int32 *key1, int32 *key2)
{
    CONTROL_CheckRange(which);

    *key1 = CONTROL_KeyMapping[which].key1;
    *key2 = CONTROL_KeyMapping[which].key2;
}

void CONTROL_PrintKeyMap(void)
{
    int i;
    for (i = 0; i < CONTROL_NUM_FLAGS; i++)
    {
        printf("function %2ld key1=%3x key2=%3x\n", i, CONTROL_KeyMapping[i].key1, CONTROL_KeyMapping[i].key2);
    }
}

void CONTROL_PrintControlFlag(int32 i)
{
    printf("function %2ld active=%ld used=%ld toggle=%ld buttonheld=%ld cleared=%ld\n",
        i, CONTROL_Flags[i].active, CONTROL_Flags[i].used, CONTROL_Flags[i].toggle,
        CONTROL_Flags[i].buttonheld, CONTROL_Flags[i].cleared);
}

void CONTROL_PrintAxes(void)
{
    int i;
    printf("numaxes=%ld\n", CONTROL_NumAxes);
    for (i = 0; i < MAXAXES; i++)
    {
        printf("axis=%ld analog=%ld digital1=%ld digital2=%ld\n", i, CONTROL_AxesMap[i].analogmap, 
            CONTROL_AxesMap[i].minmap, CONTROL_AxesMap[i].maxmap);
    }
}

void CONTROL_MapButton
(
    int32 whichfunction,
    int32 whichbutton,
    boolean doubleclicked
)
{
    CONTROL_CheckRange(whichfunction);

    if (whichbutton < 0 || whichbutton >= CONTROL_NumButtons)
        Error("CONTROL_MapButton : button %d out of valid range for %d buttons.", whichbutton, CONTROL_NumButtons);

    if (doubleclicked)
        CONTROL_DeviceButtonMapping[whichbutton].doubleclicked = whichfunction;
    else
        CONTROL_DeviceButtonMapping[whichbutton].singleclicked = whichfunction;
}

void CONTROL_GetButtonMap
(
    int32   whichfunction,
    int32* singleclicked,
    int32* doubleclicked
)
{
    *singleclicked = CONTROL_DeviceButtonMapping[whichfunction].singleclicked;
    *doubleclicked = CONTROL_DeviceButtonMapping[whichfunction].doubleclicked;
}

void CONTROL_MapAnalogAxis
(
    int32 whichaxis,
    int32 whichanalog
)
{
    if (whichaxis < 0 || whichaxis >= MAXAXES)
        Error("CONTROL_MapAnalogAxis : axis %d out of valid range for %d axes.", whichaxis, MAXAXES);
    if (whichanalog < 0 || whichaxis >= analog_maxtype)
        Error("CONTROL_MapAnalogAxis : analog function %d out of valid range for %d analog functions.", whichaxis, analog_maxtype);

    CONTROL_AxesMap[whichaxis].analogmap = whichanalog;
}

int32 CONTROL_GetAnalogAxisMap
(
    int32 whichaxis
)
{
    if (whichaxis < 0 || whichaxis >= MAXAXES)
        Error("CONTROL_GetAnalogAxisMap : axis %d out of valid range for %d axes.", whichaxis, MAXAXES);

    return CONTROL_AxesMap[whichaxis].analogmap;
}

void CONTROL_SetAnalogAxisScale
(
    int32 whichaxis,
    int32 axisscale
)
{
    if (whichaxis < 0 || whichaxis >= MAXAXES)
        Error("CONTROL_SetAnalogAxisScale : axis %d out of valid range for %d axes.", whichaxis, MAXAXES);

    CONTROL_AxesScale[whichaxis] = axisscale;
}

int32 CONTROL_GetAnalogAxisScale
(
    int32 whichaxis
)
{
    if (whichaxis < 0 || whichaxis >= MAXAXES)
        Error("CONTROL_GetAnalogAxisScale : axis %d out of valid range for %d axes.", whichaxis, MAXAXES);

    return CONTROL_AxesScale[whichaxis];
}

void CONTROL_MapDigitalAxis
(
    int32 whichaxis,
    int32 whichfunction,
    int32 direction
)
{
    CONTROL_CheckRange(whichfunction);
    if (whichaxis < 0 || whichaxis >= MAXAXES)
        Error("CONTROL_MapDigitalAxis : axis %d out of valid range for %d axes.", whichaxis, MAXAXES);
    switch (direction)
    {
        case axis_up:
        case axis_right:
            CONTROL_AxesMap[whichaxis].maxmap = whichfunction;
            break;
        case axis_down:
        case axis_left:
            CONTROL_AxesMap[whichaxis].minmap = whichfunction;
            break;
    }
}

void CONTROL_GetDigitalAxisMap
(
    int32 whichaxis,
    int32* min,
    int32* max
)
{
    if (whichaxis < 0 || whichaxis >= MAXAXES)
        Error("CONTROL_GetDigitalAxisMap : axis %d out of valid range for %d axes.", whichaxis, MAXAXES);
    *max = CONTROL_AxesMap[whichaxis].maxmap;
    *min = CONTROL_AxesMap[whichaxis].minmap;
}

void CONTROL_ClearFlags(void)
{
    int i;
    for (i = 0; i < CONTROL_NUM_FLAGS; i++)
    {
        CONTROL_Flags[i].used = 0;
    }
}

void CONTROL_ClearAssignments(void)
{
    int i;
    memset(CONTROL_DeviceButtonMapping, BUTTONUNDEFINED, sizeof(CONTROL_DeviceButtonMapping));
    memset(CONTROL_KeyMapping, KEYUNDEFINED, sizeof(CONTROL_KeyMapping));
    memset(CONTROL_Axes, 0, sizeof(CONTROL_Axes));
    for (i = 0; i < MAXAXES; i++)
    {
        CONTROL_AxesScale[i] = NORMALAXISSCALE;
    }
    memset(CONTROL_LastAxes, 0, sizeof(CONTROL_LastAxes));
    memset(CONTROL_AxesMap, AXISUNDEFINED, sizeof(CONTROL_AxesMap));
}

void CONTROL_GetDeviceButtons(void)
{
    int t = GetTime();
    int b = 0;
    int i;

    if (CONTROL_MouseEnabled)
        b = CONTROL_GetMouseButtons();
    if (CONTROL_JoystickEnabled)
        b = CONTROL_JoyButtons(CONTROL_JoystickPort);
    if (CONTROL_ExternalEnabled)
        b = CONTROL_External->buttonstate;

    for (i = 0; i < CONTROL_NumButtons; i++)
    {
        CONTROL_DeviceButtonState[i] = (b >> (uint32_t)i) & 1;
        CONTROL_ButtonClickedState[i] = 0;
        if (CONTROL_DeviceButtonState[i])
        {
            if (!CONTROL_ButtonClicked[i])
            {
                CONTROL_ButtonClicked[i] = 1;
                if (!CONTROL_ButtonClickedCount[i] || t >= CONTROL_ButtonClickedTime[i])
                {
                    CONTROL_ButtonClickedTime[i] = t + CONTROL_DoubleClickSpeed;
                    CONTROL_ButtonClickedCount[i] = 1;
                }
                else
                {
                    CONTROL_ButtonClickedState[i] = 1;
                    CONTROL_ButtonClickedTime[i] = 0;
                    CONTROL_ButtonClickedCount[i] = 2;
                }
            }
            else
            {
                if (CONTROL_ButtonClickedCount[i] == 2)
                {
                    CONTROL_ButtonClickedCount[i] = 1;
                }
            }
        }
        else
        {
            if (CONTROL_ButtonClickedCount[i] == 2)
            {
                CONTROL_ButtonClickedCount[i] = 0;
            }
            CONTROL_ButtonClicked[i] = 0;
        }
    }
}

void CONTROL_DigitizeAxis(int whichaxis)
{
    controlaxistype *axis = &CONTROL_Axes[whichaxis];
    controlaxistype *lastAxis = &CONTROL_LastAxes[whichaxis];

    if (axis->analog > 0)
    {
        if (axis->analog > THRESHOLD)
            axis->digital = 1;
        else if (axis->analog > MINTHRESHOLD && lastAxis->digital == 1)
            axis->digital = 1;
    }
    else
    {
        if (axis->analog < -THRESHOLD)
            axis->digital = -1;
        else if (axis->analog < -MINTHRESHOLD && lastAxis->digital == -1)
            axis->digital = -1;
    }
}

void CONTROL_ScaleAxis(int whichaxis)
{
    controlaxistype* axis = &CONTROL_Axes[whichaxis];

    axis->analog = FixedMulShift(CONTROL_AxesScale[whichaxis], axis->analog, 16);
}

void CONTROL_ApplyAxis(int whichaxis, ControlInfo *info)
{
    switch (CONTROL_AxesMap[whichaxis].analogmap)
    {
        case analog_turning:
            info->dyaw += CONTROL_Axes[whichaxis].analog;
            break;
        case analog_strafing:
            info->dx += CONTROL_Axes[whichaxis].analog;
            break;
        case analog_lookingupanddown:
            info->dpitch += CONTROL_Axes[whichaxis].analog;
            break;
        case analog_elevation:
            info->dy += CONTROL_Axes[whichaxis].analog;
            break;
        case analog_rolling:
            info->droll += CONTROL_Axes[whichaxis].analog;
            break;
        case analog_moving:
            info->dz += CONTROL_Axes[whichaxis].analog;
            break;
    }
}

void CONTROL_PollDevices(ControlInfo *info)
{
    int i;
    memcpy(CONTROL_LastAxes, CONTROL_Axes, sizeof(CONTROL_Axes));
    memset(CONTROL_Axes, 0, sizeof(CONTROL_Axes));
    if (CONTROL_ExternalEnabled)
        CONTROL_FillExternalInfo(info);
    memset(info, 0, sizeof(ControlInfo));

    if (CONTROL_MouseEnabled)
        CONTROL_GetMouseDelta(&CONTROL_Axes[0].analog, &CONTROL_Axes[1].analog);
    else if (CONTROL_JoystickEnabled)
    {
        CONTROL_GetJoyDelta(CONTROL_JoystickPort, &CONTROL_Axes[0].analog, &CONTROL_Axes[1].analog,
            &CONTROL_Axes[2].analog, &CONTROL_Axes[3].analog);
        CONTROL_Axes[0].analog /= 2;
        CONTROL_Axes[1].analog /= 2;
    }
    else if (CONTROL_ExternalEnabled)
        CONTROL_PollExternalControl();

    for (i = 0; i < CONTROL_NumAxes; i++)
    {
        CONTROL_DigitizeAxis(i);
        CONTROL_ScaleAxis(i);
        LIMITCONTROL(&CONTROL_Axes[0].analog);
        CONTROL_ApplyAxis(i, info);
    }
    CONTROL_GetDeviceButtons();
}

void CONTROL_AxisFunctionState(boolean* state)
{
    int i, t, b;
    for (i = 0; i < CONTROL_NumAxes; i++)
    {
        if ((t = CONTROL_Axes[i].digital) == 0)
            continue;
        if (t > 0)
        {
            b = CONTROL_AxesMap[i].minmap;
            if (b != AXISUNDEFINED)
                state[b] = 1;
        }
        else
        {
            b = CONTROL_AxesMap[i].maxmap;
            if (b != AXISUNDEFINED)
                state[b] = 1;
        }
    }
}
void CONTROL_ButtonFunctionState(boolean* state)
{
    int i, b;
    for (i = 0; i < CONTROL_NumButtons; i++)
    {
        b = CONTROL_DeviceButtonMapping[i].doubleclicked;
        if (b != BUTTONUNDEFINED)
            state[b] |= CONTROL_ButtonClickedState[i];
        b = CONTROL_DeviceButtonMapping[i].singleclicked;
        if (b != BUTTONUNDEFINED)
            state[b] |= CONTROL_DeviceButtonState[i];
    }
}

void CONTROL_GetUserInput(UserInput* info)
{
    ControlInfo ci;
    CONTROL_PollDevices(&ci);

    info->dir = dir_None;

    if (GetTime() + (ticrate * USERINPUTDELAY) / 1000 < CONTROL_UserInputDelay)
        CONTROL_UserInputDelay = -1;

    if (GetTime() >= CONTROL_UserInputDelay)
    {
        if (CONTROL_Axes[1].digital == -1)
            info->dir = dir_North;
        else if (CONTROL_Axes[1].digital == 1)
            info->dir = dir_South;
        if (CONTROL_Axes[0].digital == -1)
            info->dir = dir_West;
        else if (CONTROL_Axes[0].digital == 1)
            info->dir = dir_East;
    }

    info->button0 = CONTROL_DeviceButtonState[0];
    info->button1 = CONTROL_DeviceButtonState[1];

    if (KB_KeyPressed(sc_UpArrow) || KB_KeyPressed(sc_kpad_8))
        info->dir = dir_North;
    else if (KB_KeyPressed(sc_DownArrow) || KB_KeyPressed(sc_kpad_2))
        info->dir = dir_South;
    else if (KB_KeyPressed(sc_LeftArrow) || KB_KeyPressed(sc_kpad_4))
        info->dir = dir_West;
    else if (KB_KeyPressed(sc_RightArrow) || KB_KeyPressed(sc_kpad_6))
        info->dir = dir_East;

    if (KB_KeyPressed(BUTTON0_SCAN_1) || KB_KeyPressed(BUTTON0_SCAN_2) || KB_KeyPressed(BUTTON0_SCAN_3))
    {
        info->button0 = 1;
    }
    if (KB_KeyPressed(BUTTON1_SCAN))
    {
        info->button1 = 1;
    }

    if (CONTROL_UserInputCleared[1])
    {
        if (!info->button0)
            CONTROL_UserInputCleared[1] = 0;
        else
            info->button0 = 0;
    }
    if (CONTROL_UserInputCleared[2])
    {
        if (!info->button1)
            CONTROL_UserInputCleared[2] = 0;
        else
            info->button1 = 0;
    }
}

void CONTROL_ClearUserInput(UserInput* info)
{
    switch (info->dir)
    {
        case dir_North:
            CONTROL_UserInputCleared[0] = 1;
            CONTROL_UserInputDelay = GetTime() + (ticrate * USERINPUTDELAY) / 1000;
            KB_ClearKeyDown(sc_UpArrow);
            KB_ClearKeyDown(sc_kpad_8);
            break;
        case dir_South:
            CONTROL_UserInputCleared[0] = 1;
            CONTROL_UserInputDelay = GetTime() + (ticrate * USERINPUTDELAY) / 1000;
            KB_ClearKeyDown(sc_DownArrow);
            KB_ClearKeyDown(sc_kpad_2);
            break;
        case dir_East:
            CONTROL_UserInputCleared[0] = 1;
            CONTROL_UserInputDelay = GetTime() + (ticrate * USERINPUTDELAY) / 1000;
            KB_ClearKeyDown(sc_LeftArrow);
            KB_ClearKeyDown(sc_kpad_4);
            break;
        case dir_West:
            CONTROL_UserInputCleared[0] = 1;
            CONTROL_UserInputDelay = GetTime() + (ticrate * USERINPUTDELAY) / 1000;
            KB_ClearKeyDown(sc_RightArrow);
            KB_ClearKeyDown(sc_kpad_6);
            break;
    }
    if (info->button0)
        CONTROL_UserInputCleared[1] = 1;
    if (info->button1)
        CONTROL_UserInputCleared[2] = 1;
}

void CONTROL_ClearButton(int32 whichbutton)
{
    CONTROL_CheckRange(whichbutton);
    BUTTONCLEAR(whichbutton);

    CONTROL_Flags[whichbutton].cleared = 1;
}

void CONTROL_GetInput(ControlInfo* info)
{
    int i;
    boolean state[CONTROL_NUM_FLAGS];
    int pressed;

    //ServiceEvents(); // VID
    Sys_HandleEvents();

    CONTROL_PollDevices(info);

    memset(state, 0, sizeof(state));
    CONTROL_ButtonFunctionState(state);
    CONTROL_AxisFunctionState(state);

    CONTROL_ButtonHeldState1 = CONTROL_ButtonState1;
    CONTROL_ButtonHeldState2 = CONTROL_ButtonState2;

    CONTROL_ButtonState1 = 0;
    CONTROL_ButtonState2 = 0;

    for (i = 0; i < CONTROL_NUM_FLAGS; i++)
    {
        pressed = CONTROL_KeyboardFunctionPressed(i);
        pressed |= state[i];
        CONTROL_SetFlag(i, pressed);
        if (CONTROL_Flags[i].cleared)
        {
            if (!CONTROL_Flags[i].active)
                CONTROL_Flags[i].cleared = 0;
        }
        else
            BUTTONSET(i, CONTROL_Flags[i].active);
    }
}

void CONTROL_WaitRelease(void)
{
    UserInput info;

    do
    {
        CONTROL_GetUserInput(&info);
        if (info.dir == dir_None && !info.button0 && !info.button1)
            break;
    } while (1);
}

void CONTROL_Ack(void)
{
    UserInput info, info2;
    int a, b;
    CONTROL_GetUserInput(&info2);

    do
    {
        CONTROL_GetUserInput(&info);
        if (!info.button0)
            info2.button0 = 0;
        if (!info.button1)
            info2.button1 = 0;

        a = !info2.button0 && info.button0;
        b = !info2.button1 && info.button1;
    } while (!a && !b);
}

void CONTROL_Startup
(
    controltype which,
    int32(*TimeFunction)(void),
    int32 ticspersecond
)
{
    if (CONTROL_Started == true)
        return;

    if (TimeFunction)
        GetTime = TimeFunction;
    else
        GetTime = CONTROL_GetTime;

    ticrate = ticspersecond;

    CONTROL_DoubleClickSpeed = (ticrate * 57) / 100;
    if (CONTROL_DoubleClickSpeed < 1)
        CONTROL_DoubleClickSpeed = 1;

    CONTROL_FlightStickMode = false;
    CONTROL_HatEnabled = false;
    CONTROL_ThrottleEnabled = false;
    CONTROL_NumAxes = 0;
    CONTROL_MouseEnabled = false;

    if (CONTROL_JoystickPort)
    {
        switch (which)
        {
            case controltype_keyboardandgamepad:
            case controltype_keyboardandflightstick:
            case controltype_keyboardandthrustmaster:
                which = controltype_keyboardandjoystick;
                break;
        }
    }

    switch (which)
    {
        case controltype_keyboard:
            printf("CONTROL_Startup: Keyboard Mode\n");
            break;
        case controltype_keyboardandmouse:
            CONTROL_MousePresent = CONTROL_StartMouse();
            if (!CONTROL_MousePresent)
            {
                printf("CONTROL_Startup: Mouse not Found!\n");
                printf("CONTROL_Startup: Using Keyboard Mode\n");
            }
            else
            {
                printf("CONTROL_Startup: Mouse Present\n");
                CONTROL_NumAxes = 2;
                CONTROL_MouseEnabled = true;
            }
            break;
        case controltype_keyboardandflightstick:
            if (CONTROL_StartJoy(CONTROL_JoystickPort))
            {
                printf("CONTROL_Startup: FlightStick Present\n");
                CONTROL_FlightStickMode = true;
                CONTROL_ThrottleEnabled = true;
                CONTROL_NumAxes = 4;
                CONTROL_JoystickEnabled = true;
            }
            else
            {
                printf("CONTROL_Startup: FlighStick not Found!\n");
                printf("CONTROL_Startup: Using Keyboard Mode\n");
            }
            break;
        case controltype_keyboardandgamepad:
            if (CONTROL_StartJoy(CONTROL_JoystickPort))
            {
                printf("CONTROL_Startup: GamePad Present\n");
                CONTROL_NumAxes = 2;
                CONTROL_JoystickEnabled = true;
            }
            else
            {
                printf("CONTROL_Startup: GamePad not Found!\n");
                printf("CONTROL_Startup: Using Keyboard Mode\n");
            }
            break;
        case controltype_keyboardandjoystick:
            if (CONTROL_StartJoy(CONTROL_JoystickPort))
            {
                printf("CONTROL_Startup: Joystick Present\n");
                CONTROL_NumAxes = 4;
                CONTROL_JoystickEnabled = true;
            }
            else
            {
                printf("CONTROL_Startup: Joystick not Found!\n");
                printf("CONTROL_Startup: Using Keyboard Mode\n");
            }
            break;
        case controltype_keyboardandthrustmaster:
            if (CONTROL_StartJoy(CONTROL_JoystickPort))
            {
                printf("CONTROL_Startup: ThrustMaster Present\n");
                CONTROL_HatEnabled = true;
                CONTROL_NumAxes = 4;
                CONTROL_JoystickEnabled = true;
            }
            else
            {
                printf("CONTROL_Startup: ThrustMaster not Found!\n");
                printf("CONTROL_Startup: Using Keyboard Mode\n");
            }
            break;
        case controltype_keyboardandexternal:
            CONTROL_NumAxes = 6;
            CONTROL_ExternalStartup();
            break;
    }

    CONTROL_ButtonState1 = 0;
    CONTROL_ButtonState2 = 0;
    CONTROL_ButtonHeldState1 = 0;
    CONTROL_ButtonHeldState2 = 0;
    memset(CONTROL_UserInputCleared, 0, sizeof(CONTROL_UserInputCleared));
    CONTROL_ClearFlags();
    if (CONTROL_ExternalEnabled)
        CONTROL_ExternalSetup();

    CONTROL_Started = true;
}

void CONTROL_Shutdown()
{
    if (!CONTROL_Started)
        return;
    MOUSE_Shutdown();
    CONTROL_Started = false;
}
