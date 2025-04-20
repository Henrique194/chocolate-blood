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
#include <string.h>
#include "typedefs.h"
#include "getopt.h"

const char *OptArgv[16];
int OptArgc;
const char *OptFull;
const char *SwitchChars = "-/";

extern "C" {
extern int sys_argc;
extern const char **sys_argv;
}

int GetOptions(SWITCH *switches)
{
    static const char *pChar = NULL;
    static int OptIndex = 1;
    if (!pChar)
    {
        if (OptIndex >= sys_argc)
            return -1;
        pChar = sys_argv[OptIndex++];
        if (!pChar)
            return -1;
    }
    OptFull = pChar;
    if (!strchr(SwitchChars, *pChar))
    {
        pChar = NULL;
        return -2;
    }
    pChar++;
    if (!*pChar)
        return -3;
    int i;
    int vd;
    int nLength;
    for (i = 0; 1; i++)
    {
        if (!switches[i].name)
            return -3;
        nLength = strlen(switches[i].name);
        if (!strnicmp(pChar, switches[i].name, nLength))
        {
            vd = switches[i].at4;
            pChar += nLength;
            if (!*pChar)
            {
                pChar = NULL;
            }
            OptArgc = 0;
            while (OptArgc < switches[i].at8)
            {
                if (!pChar)
                {
                    if (OptIndex >= sys_argc)
                        break;
                    pChar = sys_argv[OptIndex++];
                }
                if (strchr(SwitchChars, *pChar) != 0)
                    break;
                OptArgv[OptArgc++] = pChar;
                pChar = NULL;
            }
            return vd;
        }
    }
}
