/*
 * Copyright (C) 2018, 2022 nukeykt
 *
 * This file is part of Blood-RE.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include "typedefs.h"

#pragma pack(push, 1)

union BUTTONFLAGS
{
    char            byte;
    struct
    {
        unsigned int jump           : 1;    // player is jumping (once!)
        unsigned int crouch         : 1;    // player is crouching
        unsigned int shoot          : 1;    // normal attack
        unsigned int shoot2         : 1;    // alternate attack
        unsigned int lookUp         : 1;    // > glance or aim up/down
        unsigned int lookDown       : 1;    // > if glancing then lookCenter is set
    };
};

union KEYFLAGS
{
    short           word;
    struct
    {
        unsigned int action         : 1;    // open or activate
        unsigned int jab            : 1;    // quick attack
        unsigned int prevItem       : 1;    // next inventory item
        unsigned int nextItem       : 1;    // prev inventory item
        unsigned int useItem        : 1;    // use inventory item
        unsigned int prevWeapon     : 1;    // prev useable weapon
        unsigned int nextWeapon     : 1;    // next useable weapon
        unsigned int holsterWeapon  : 1;    // holster current weapon

        unsigned int lookCenter     : 1;    // used for lookUp/lookDown only
        unsigned int lookLeft       : 1;    // > glance or aim up/down
        unsigned int lookRight      : 1;    // > if glancing then lookCenter is set
        unsigned int spin180        : 1;    // spin 180 degrees

        unsigned int pause          : 1;    // pause the game
        unsigned int quit           : 1;    // quit the game
        unsigned int restart        : 1;    // restart the level
    };
};

union USEFLAGS
{
    char            byte;
    struct
    {
        unsigned int useBeastVision     : 1;
        unsigned int useCrystalBall     : 1;
        unsigned int useJumpBoots       : 1;
        unsigned int useMedKit          : 1;
    };
};

union SYNCFLAGS
{
    char            byte;
    struct
    {
        unsigned int buttonChange   : 1;
        unsigned int keyChange      : 1;
        unsigned int useChange      : 1;
        unsigned int weaponChange   : 1;
        unsigned int mlookChange    : 1;
        unsigned int run            : 1;    // player is running
    };
};

struct QINPUT
{
    SYNCFLAGS       syncFlags;              // always sent: indicates optional fields
    schar           forward;                // always sent
    sshort          turn;                   // always sent
    schar           strafe;                 // always sent

    // optional fields
    BUTTONFLAGS     buttonFlags;
    KEYFLAGS        keyFlags;
    USEFLAGS        useFlags;
    uchar           newWeapon;      // sent as 0 every frame unless changed
    schar           mlook;
};

#pragma pack(pop)

extern QINPUT gInput;
extern QBOOL bSilentAim;

void func_2906C(void);

void ctrlInit(void);

void ctrlGetInput(void);
void ctrlTerm(void);

#endif
