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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helix.h"
#include "video.h"

VGT Video;
int gColor;
int gROP;
int gError;
int gPages;
PAGE_STRUCT gPageTable[4];
int gYLookup[1200];
int nTextMode;

const char *ModelName[] = {
    "0:MONO",
    "1:PLANAR",
    "2:PACKEDPIXEL",
    "3:CHAIN256",
    "4:NONCHAIN256"
};

void InstallDriver(VGT **vgt)
{
    Video = **vgt;
    nTextMode = gGetMode();
    Video.Init();
}


void gRestoreMode(void)
{
    gSetMode(nTextMode);
}

extern VGT vgt13;

VGT* vgtList[] = { &vgt13, NULL };

int gFindMode(int a1, int a2, int a3, int a4)
{
    int v8 = 0;
    while (a3 >>= 1)
    {
        v8++;
    }
    
    for (VGT *v4 = vgtList[0]; v4 != NULL; v4++)
    {
        if (v4->xRes == a1 && v4->yRes == a2 && v4->cRes == v8 && v4->model == a4)
        {
            InstallDriver(&v4);
            return TRUE;
        }
    }
    return FALSE;
}

void gEnumDrivers(void)
{
    int v8 = 1;
    for (VGT *v4 = vgtList[0]; v4 != NULL; v4++)
        printf("%2i: %-30s (%ix%ix%i %s) \n", v8++, v4->name, v4->xRes, v4->yRes, v4->cRes, ModelName[v4->model]);
}


// mode 13 helix

int MCGAValid(void);
char MCGAName[] = "MCGA 320x200 256 Color";
void MCGAInit(void);
void MCGASetMode(void);
void MCGASetPage(int, int, int, int);
int MCGAGetPage(void);
void MCGAClear(int);
void MCGASetPixel(int, int, int);
int MCGAGetPixel(int, int, int);
void MCGAHLine(int, int, int, int);
void MCGAVLine(int, int, int, int);
void MCGALine(int, int, int, int, int);
void MCGAFillBox(int, int, int, int, int);
void MCGAHLineROP(int, int, int, int);
void MCGAVLineROP(int, int, int, int);
void MCGABlitRLE2V(QBITMAP*, int, int, int);
void MCGABlitV2M(int, int, int, int, int, QBITMAP*, int, int);
void MCGABlitM2V(BYTE*, int, int, int, int, int, int);
void MCGABlitMT2V(BYTE*, int, int, int, int, int, int, int);
void MCGABlitMono(BYTE*, int, int, int, int, int, int, int);

static int MCGAPage;

VGT vgt13 = {
    MCGAValid,
    MCGAName,
    320,
    200,
    8,
    3,
    MCGAInit,
    MCGASetMode,
    MCGASetPage,
    MCGAGetPage,
    MCGAClear,
    MCGASetPixel,
    MCGAGetPixel,
    MCGAHLine,
    MCGAVLine,
    MCGALine,
    MCGAFillBox,
    MCGAHLineROP,
    MCGAVLineROP,
    MCGABlitRLE2V,
    MCGABlitV2M,
    MCGABlitM2V,
    MCGABlitMT2V,
    MCGABlitMono
};

PAGE_STRUCT MCGAPageTable[] = {
    { 1, 0xa0000, 0xfa00, 320, 200, 320 },
    { 0, 0xafa00, 0x500, 320, 4, 320 }
};

int MCGAValid(void)
{
    return 1;
}

void MCGAInit(void)
{
    gPages = 2;
    memcpy(gPageTable, MCGAPageTable, sizeof(MCGAPageTable));
    int offset = 0;
    for (int i = 0; i < 1200; i++)
    {
        gYLookup[i] = offset;
        offset += MCGAPageTable[0].bytesPerRow;
    }
}

void MCGASetMode(void)
{
    Video_Set(1, 320, 200);
    MCGASetPage(0, 0, 0, 1);
}

void MCGASetPage(int a, int b, int c, int d)
{
    MCGAPage = a;

    // ...
}

int MCGAGetPage(void)
{
    return MCGAPage;
}

void MCGAClear(int page)
{
    if (page == 0)
        memset((char*)MCGAPageTable[0].begin, 0, MCGAPageTable[0].size);
}

void MCGASetPixel(int page, int x, int y)
{
    if (page == 0)
        ((char*)MCGAPageTable[0].begin)[gYLookup[y] + x] = gColor;
}

int MCGAGetPixel(int page, int x, int y)
{
    if (page == 0)
        return ((char*)MCGAPageTable[0].begin)[gYLookup[y] + x]; // fixme: word read
    return 0;
}
void MCGAHLine(int page, int a1, int a2, int a3)
{
    if (page != 0)
        return;
    char* p = (char*)(MCGAPageTable[0].begin + gYLookup[a1] + a2);
    int cnt = a3 - a2 + 1;
    while (cnt--)
    {
        *p++ = gColor;
    }
}

void MCGAVLine(int page, int a1, int a2, int a3)
{
    if (page != 0)
        return;
    char* p = (char*)(MCGAPageTable[0].begin + gYLookup[a2] + a1);
    int stride = gPageTable[0].bytesPerRow;
    int cnt = a3 - a2 + 1;
    do
    {
        *p = gColor;
        *p += stride;
    } while (--cnt);
}

void MCGAFillBox(int page, int a1, int a2, int a3, int a4)
{
    if (page != 0)
        return;
    char* p = (char*)(MCGAPageTable[0].begin + gYLookup[a2] + a1);

    int cnt = a4 - a2 + 1;
    while (cnt--)
    {
        memset(p, gColor, a3 - a1 + 1);
        p += MCGAPageTable[0].bytesPerRow;
    }
}

void MCGALine(int page, int a1, int a2, int a3, int a4)
{
    if (a2 >= a4)
    {
        int t;
        t = a2; a2 = a4; a4 = t;
        t = a1; a1 = a3; a3 = t;
    }

    char* p = (char*)(MCGAPageTable[0].begin + gYLookup[a2] + a1);

    int dx = a3 - a1;
    int dy = a4 - a2;
    int dir = 1;
    if (dx < 0)
    {
        dir = -1;
        dx = -dx;
    }

    int cnt;
    int add;
    int pinc0;
    int pinc1;
    if (dx < dy)
    {
        add = dx;
        cnt = dy;
        pinc0 = 320;
        pinc1 = pinc0 + dir;
    }
    else
    {
        add = dy;
        cnt = dx;
        pinc0 = dir;
        pinc1 = pinc0 + 320;
    }


    int frac = -cnt;

    int finc0 = -2 * cnt;
    add *= 2;

    while (cnt--)
    {
        *p = 0xff;

        frac += add;
        if (add != 0 && frac >= 0)
        {
            p += pinc1;
            frac += finc0;
        }
        else
            p += pinc1;
    }

}

void MCGAHLineROP(int page, int a1, int a2, int a3)
{
    if (page != 0)
        return;
    char* p = (char*)(MCGAPageTable[0].begin + gYLookup[a1] + a2);
    int cnt = a3 - a2 + 1;
    switch (gROP)
    {
        case 0:
            while (cnt--)
            {
                *p++ = gColor;
            }
            break;
        case 1:
            while (cnt--)
            {
                *p++ &= gColor;
            }
            break;
        case 2:
            while (cnt--)
            {
                *p++ |= gColor;
            }
            break;
        case 3:
            while (cnt--)
            {
                *p++ ^= gColor;
            }
            break;
    }
}

void MCGAVLineROP(int page, int a1, int a2, int a3)
{
    if (page != 0)
        return;
    char* p = (char*)(MCGAPageTable[0].begin + gYLookup[a2] + a1);
    int stride = gPageTable[0].bytesPerRow;
    int cnt = a3 - a2 + 1;
    switch (gROP)
    {
        case 0:
            do
            {
                *p = gColor;
                *p += stride;
            } while (--cnt);
            break;
        case 1:
            do
            {
                *p &= gColor;
                *p += stride;
            } while (--cnt);
            break;
        case 2:
            do
            {
                *p |= gColor;
                *p += stride;
            } while (--cnt);
            break;
        case 3:
            do
            {
                *p ^= gColor;
                *p += stride;
            } while (--cnt);
            break;
    }
}

void MCGABlitRLE2V(QBITMAP* a1, int page, int a2, int a3)
{
    if (page != 0)
        return;
    char* p = (char*)(MCGAPageTable[0].begin + gYLookup[a3] + a2);
    BYTE* s = a1->data;
    int cnt = a1->rows + 1;
    while (--cnt)
    {
        char* d = p;
        while (1)
        {
            BYTE p1 = *s++;
            BYTE p2 = *s++;
            if (!s)
                break;
            memcpy(d, s, p2);
        }
        p += MCGAPageTable[0].bytesPerRow;
    }
}

void MCGABlitV2M(int page, int a1, int a2, int a3, int a4, QBITMAP *a5, int a6, int a7)
{
    if (page != 0)
        return;
    char* p = (char*)(MCGAPageTable[0].begin + gYLookup[a3] + a2);
    BYTE* d = &a5->data[a5->stride * a7 + a6];

    int cnt = a4 - a2;
    int w = a3 - a1;
    do
    {
        memcpy(d, p, w);

        d += a5->stride;
        p += MCGAPageTable[0].bytesPerRow;

    } while (--cnt);
}

void MCGABlitM2V(BYTE* a1, int a2, int a3, int a4, int page, int a5, int a6)
{
    if (page != 0)
        return;
    char* p = (char*)(MCGAPageTable[0].begin + gYLookup[a6] + a5);

    int cnt = a4;
    do
    {
        memcpy(p, a1, a3);

        a1 += a2;
        p += MCGAPageTable[0].bytesPerRow;

    } while (--cnt);

}

void MCGABlitMT2V(BYTE* a1, int a2, int a3, int a4, int a5, int page, int a6, int a7)
{
    if (page != 0)
        return;
    char* p = (char*)(MCGAPageTable[0].begin + gYLookup[a7] + a6);

    int cnt = a5;
    do
    {
        for (int i = 0; i < a4; i++)
        {
            if (a1[i] != (BYTE)a2)
                p[i] = a1[i];
        }

        a1 += a3;
        p += MCGAPageTable[0].bytesPerRow;

    } while (--cnt);
}

void MCGABlitMono(BYTE* a1, int a2, int a3, int a4, int a5, int page, int a6, int a7)
{
    if (page != 0)
        return;
    char* p = (char*)(MCGAPageTable[0].begin + gYLookup[a7] + a6);

    char m = a2;
    int cnt = a5;
    do
    {
        for (int i = 0; i < a4; i++)
        {
            if (a1[i] & m)
                p[i] = gColor;
        }
        if (m & 0x80)
        {
            m <<= 1;
            m |= 1;
            a1 += a3;
        }
        else
            m <<= 1;
        p += MCGAPageTable[0].bytesPerRow;
    } while (--cnt);
}

