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
#ifndef _HELIX_H_
#define _HELIX_H_
#include "typedefs.h"

struct QBITMAP {
    uchar bitModel;
    uchar tcolor;
    ushort cols;
    ushort rows;
    ushort stride;
    ushort xOrg;
    ushort yOrg;
    BYTE data[1];
};

#ifdef __cplusplus
extern "C" {
#endif
extern int gColor;
#ifdef __cplusplus
}
#endif

struct VGT {
    int (*Valid)(void);
    char *name;
    int xRes;
    int yRes;
    int cRes;
    int model;
    void (*Init)(void);
    void (*SetMode)(void);
    void (*SetPage)(int, int, int, int);
    int (*GetPage)(void);
    void (*Clear)(int);
    void (*SetPixel)(int, int, int);
    int (*GetPixel)(int, int, int);
    void (*HLine)(int, int, int, int);
    void (*VLine)(int, int, int, int);
    void (*Line)(int, int, int, int, int);
    void (*FillBox)(int, int, int, int, int);
    void (*HLineROP)(int, int, int, int);
    void (*VLineROP)(int, int, int, int);
    void (*BlitRLE2V)(QBITMAP *, int, int, int);
    void (*BlitV2M)(int, int, int, int, int, QBITMAP*, int, int);
    void (*BlitM2V)(BYTE*, int, int, int, int, int, int);
    void (*BlitMT2V)(BYTE*, int, int, int, int, int, int, int);
    void (*BlitMono)(BYTE*, int, int, int, int, int, int, int);

    static inline void SetColor(int color) { gColor = color; }
};

struct PAGE_STRUCT {
    unsigned int flags;
    uintptr_t begin;
    unsigned int size;
    unsigned int width;
    unsigned int height;
    unsigned int bytesPerRow;
    unsigned int pad[2];
};


#ifdef __cplusplus
extern "C" {
#endif
extern VGT Video;
extern int gROP;
extern int gError;
extern int gPages;
extern PAGE_STRUCT gPageTable[4];
extern int gYLookup[1200];
void gRestoreMode(void);
int gFindMode(int, int, int, int);
int gGetMode(void);
int gSetMode(int);
void gEnumDrivers(void);
void gSetDACRange(int, int, byte*);
void gSetDAC(int, int, int, int);
#ifdef __cplusplus
}
#endif


#endif
