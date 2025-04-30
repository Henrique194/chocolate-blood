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
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <memory.h>
#include <string.h>
#include "typedefs.h"
#include "debug4g.h"
#include "misc.h"


char *ReadLine(char *line, int bytes, char **buf)
{
    int i = 0;
    if (!buf || !*buf || !**buf)
        return NULL;
    while (i < bytes && **buf != '\0' && **buf != '\n')
    {
        line[i] = **buf;
        (*buf)++;
        i++;
    }
    if (**buf == '\n' && i < bytes)
    {
        line[i++] = **buf;
        (*buf)++;
    }
    else
    {
        while (**buf != '\0' && **buf != '\n')
        {
            (*buf)++;
        }
        if (**buf == '\n')
            (*buf)++;
    }
    if (i < bytes)
        line[i] = '\0';
    return *buf;
}

QBOOL FileRead(int hFile, void *buffer, uint32_t length)
{
    return read(hFile, buffer, length) == length;
}

QBOOL FileWrite(int hFile, void *buffer, uint32_t length)
{
    return write(hFile, buffer, length) == length;
}

QBOOL FileLoad(char *name, void *buffer, uint32_t length)
{
    dassert(buffer != NULL, 98);
    int hFile = open(name, O_BINARY);
    if (hFile == -1)
        return FALSE;
    uint32_t l = read(hFile, buffer, length);
    close(hFile);
    return l == length;
}

QBOOL FileSave(char *name, void *buffer, uint32_t length)
{
    dassert(buffer != NULL, 121);
    int hFile = open(name, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IWRITE);
    if (hFile == -1)
        return FALSE;
    uint32_t l = write(hFile, buffer, length);
    close(hFile);
    return l == length;
}

struct MEMINFO {
    uint f_0;
    char __f_4[0x4];
    uint f_8;
    char __f_c[0x8];
    uint f_14;
    uint f_18;
    uint f_1c;
    char __f_20[0x10];
};

void AddExtension(char *name, const char *ext)
{
    char buf[_MAX_PATH];
    const char *dir, *fn, *oext;
    MySplitPath(name, buf, &dir, &fn, &oext);
    if (!*oext)
        oext = ext;
    strcpy(name, dir);
    strcat(name, fn);
    strcat(name, oext);
}

void ChangeExtension(char *name, const char *ext)
{
    char buf[148];
    const char *dir, *fn, *oext, *drive;
    MySplitPath(name, buf, &dir, &fn, &oext);
    oext = ext;
    strcpy(name, dir);
    strcat(name, fn);
    strcat(name, oext);
}

uint32_t randSeed = 1;

static uint32_t randStep(uint32_t seed)
{
    if (seed & 0x80000000)
    {
        seed <<= 1;
        seed ^= 0x20000004;
        seed |= 1;
    }
    else
        seed <<= 1;
    return seed;
}

uint32_t qrand(void)
{
    randSeed = randStep(randSeed);
    return randSeed & 0x7fff;
}
