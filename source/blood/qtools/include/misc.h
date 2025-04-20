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
#ifndef _MISC_H_
#define _MISC_H_

#include <stdlib.h>
#include <string.h>
#include "typedefs.h"

char *ReadLine(char *, int, char **);
QBOOL FileRead(int, void*, uint32_t);
QBOOL FileLoad(char*, void*, uint32_t);

void ChangeExtension(char *name, char *ext);

uint32_t qrand(void);

uint32_t func_A8B30(void);
uint32_t func_A8B50(void);

inline int Min(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}

inline int Max(int a, int b)
{
    if (a < b)
        return b;
    else
        return a;
}

inline void SetBitString(byte *pArray, int nIndex)
{
    pArray[nIndex>>3] |= 1<<(nIndex&7);
}

inline void ClearBitString(byte *pArray, int nIndex)
{
    pArray[nIndex >> 3] &= ~(1 << (nIndex & 7));
}

inline byte TestBitString(byte *pArray, int nIndex)
{
    return pArray[nIndex>>3] & (1<<(nIndex&7));
}

inline int scale(int a1, int a2, int a3, int a4, int a5)
{
    return (a1-a2) * (a5-a4) / (a3-a2) + a4;
}

inline int IncRotate(int a1, int a2)
{
    a1++;
    if (a1 >= a2)
        a1 = 0;
    return a1;
}

inline int DecRotate(int a1, int a2)
{
    a1--;
    if (a1 < 0)
        a1 += a2;
    return a1;
}

inline int IncBy(int a, int b)
{
    a += b;
    int q = a % b;
    a -= q;
    if (q < 0)
        a -= b;
    return a;
}

inline int DecBy(int a, int b)
{
    a--;
    int q = a % b;
    a -= q;
    if (q < 0)
        a -= b;
    return a;
}

inline int ClipLow(int a, int b)
{
    if (a < b)
        return b;
    else
        return a;
}

inline int ClipHigh(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}

inline int ClipRange(int a, int b, int c)
{
    if (a < b)
        return b;
    if (a > c)
        return c;
    return a;
}

struct POINT2D {
    int x, y;
};

struct POINT3D {
    int x, y, z;
};

struct VECTOR2D {
    int dx, dy;
};

struct VECTOR3D {
    int32_t dx, dy, dz;
};



static inline int dmulscale30r(int a, int b, int c, int d)
{
    int64_t mul = (int64_t)a * (int64_t)b + (int64_t)c * (int64_t)d;
    mul += 0x20000000;
    return (int)(mul >> 30);
}

static inline int klabs(int a)
{
    if (a < 0)
        return -a;
    return a;
}

static inline int isneg(int a)
{
    return (a < 0);
}

static inline int kscale(int a, int b, int c)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    return (int)(mul / c);
}

static inline int ksgn(int a)
{
    return (a > 0) - (a < 0);
}

static inline int mulscale8(int a, int b)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    return (int)(mul >> 8);
}

static inline int mulscale14(int a, int b)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    return (int)(mul >> 14);
}

static inline int mulscale16(int a, int b)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    return (int)(mul >> 16);
}

static inline int mulscale24(int a, int b)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    return (int)(mul >> 24);
}

static inline int mulscale28(int a, int b)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    return (int)(mul >> 28);
}

static inline int mulscale30(int a, int b)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    return (int)(mul >> 14);
}

static inline int interpolate16(int a, int b, int c)
{
    int64_t mul = (int64_t)(b - a) * (int64_t)c;
    return a + (int)(mul >> 16);
}

static inline int divscale16(int a, int b)
{
    int64_t d = ((int64_t)a << 16) / b;
    return (int)d;
}

static inline int divscale24(int a, int b)
{
    int64_t d = ((int64_t)a << 24) / b;
    return (int)d;
}

static inline int divscale24(int a, int b, int c)
{
    int64_t d = ((int64_t)a << c) / b;
    return (int)d;
}

extern "C" {
void Sys_WaitVSync();
};

static inline void WaitVBL(void)
{
    Sys_WaitVSync();
}

static inline int approxDist(int a, int b)
{
    uint32_t a2 = klabs(a);
    uint32_t b2 = klabs(b);

    if (a2 <= b2)
        a2 = (a2 * 3) >> 3;
    else
        b2 = (b2 * 3) >> 3;
    return a2 + b2;
}

static inline int mulscale16r(int a, int b)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    mul += 0x8000;
    return (int)(mul >> 16);
}

static inline int mulscale30r(int a, int b)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    mul += 0x20000000;
    return (int)(mul >> 30);
}

static inline int dmulscale(int a, int b, int c, int d, int e)
{
    int64_t mul = (int64_t)a * (int64_t)b + (int64_t)c * (int64_t)d;
    return (int)(mul >> e);
}

static inline int dmulscale16(int a, int b, int c, int d)
{
    int64_t mul = (int64_t)a * (int64_t)b + (int64_t)c * (int64_t)d;
    return (int)(mul >> 16);
}

static inline int dmulscale30(int a, int b, int c, int d)
{
    int64_t mul = (int64_t)a * (int64_t)b + (int64_t)c * (int64_t)d;
    return (int)(mul >> 30);
}

static inline int dmulscale32(int a, int b, int c, int d)
{
    int64_t mul = (int64_t)a * (int64_t)b + (int64_t)c * (int64_t)d;
    return (int)(mul >> 32);
}

static inline int mulscale(int a, int b, int c)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    return (int)(mul >> c);
}

static inline int tmulscale16(int a, int b, int c, int d, int e, int f)
{
	int64_t mul = (int64_t)a * (int64_t)b + (int64_t)c * (int64_t)d + (int64_t)e * (int64_t)f;
	return (int)(mul >> 16);
}

static inline void debugTrap(void)
{
}

inline int QRandom(int n)
{
    return mulscale(qrand(), n, 15);
}

inline int QRandom2(int n)
{
    return mulscale(qrand(), n, 14) - n;
}

inline QBOOL Chance(int a1)
{
    return rand() < (a1>>1);
}

inline uint Random(int a1)
{
    return mulscale(rand(), a1, 15);
}

inline int Random2(int a1)
{
    return mulscale(rand(), a1, 14)-a1;
}

inline int Random3(int a1)
{
    return mulscale(rand()+rand(), a1, 15) - a1;
}

static inline void MySplitPath(const char* s, char *buf, char** dir, char** name, char** ext)
{
    int l = strlen(s);

    *buf = '\0';
    if (ext)
        *ext = buf;
    if (name)
        *ext = buf;
    if (ext)
        *ext = buf;
    buf++;

    for (int i = l - 1; i >= 0; i--)
    {
        if (s[i] == '.')
        {
            if (ext)
            {
                *ext = buf;
                memcpy(buf, s + i, l - i);
                buf += l - i;
            }
            l = i;
        }
        else if (s[i] == '\\' || buf[i] == '/')
        {
            if (name)
            {
                *name = buf;
                memcpy(buf, s + i + 1, l - i - 1);
                buf += l - i - 1;
            }
            if (dir)
            {
                *dir = buf;
                memcpy(buf, s, i + 1);
            }
            return;
        }
    }
    if (name)
    {
        *name = buf;
        memcpy(buf, s, l);
    }
}

#endif // !_MISC_H_
