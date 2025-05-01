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
#include "typedefs.h"
#include "engine.h"
#include "crc32.h"
#include "globals.h"
#include "misc.h"
#include "resource.h"
#include "screen.h"
#include "tile.h"


void uninitcache()
{
}

extern "C" void agecache_replace()
{
}

extern "C" void initcache_replace(intptr_t, int32_t)
{
}

extern "C" void allocache_replace(intptr_t*, int32_t, char*)
{
}

extern "C" void *kmalloc_replace(size_t size)
{
    return Resource::Alloc(size);
}

extern "C" void kfree_replace(void *pMem)
{
    Resource::Free(pMem);
}

extern "C" int loadpics_replace(char*)
{
    return tileInit(0, NULL) ? 0 : - 1;
}

extern "C" void loadtile_replace(short nTile)
{
    tileLoadTile(nTile);
}

extern "C" intptr_t allocatepermanenttile_replace(short a1, int a2, int a3)
{
    return (intptr_t)tileAllocTile(a1, a2, a3);
}

void overwritesprite (int32_t thex, int32_t they, short tilenum,
    signed char shade, char stat, char dapalnum)
{
   rotatesprite(thex<<16,they<<16,65536L,(stat&8)<<7,tilenum,shade,dapalnum,
      ((stat&1^1)<<4)+(stat&2)+((stat&4)>>2)+((stat&16)>>2)^((stat&8)>>1),
      windowx1,windowy1,windowx2,windowy2);
}

enum {
    kFakevarFlat = 0x8000,
    kFakevarMask = 0xc000,
};

extern "C" int animateoffs_replace(short a1, ushort a2)
{
    int offset = 0;
    int frames;
    int vd;
    if (a1 < 0 || a1 >= kMaxTiles)
        return offset;
    frames = picanm[a1].animframes;
    if (frames > 0)
    {
        if ((a2&0xc000) == 0x8000)
            vd = (CRC32(&a2, 2)+gFrameClock)>>picanm[a1].animspeed;
        else
            vd = gFrameClock>>picanm[a1].animspeed;
        switch (picanm[a1].animtype)
        {
        case 1:
            offset = vd % (2*frames);
            if (offset >= frames)
                offset = 2*frames-offset;
            break;
        case 2:
            offset = vd % (frames+1);
            break;
        case 3:
            offset = -(vd % (frames+1));
            break;
        }
    }
    return offset;
}

extern "C" void uninitengine_replace()
{
    tileTerm();
}

extern "C" void loadpalette_replace()
{
    scrLoadPalette();
}

extern "C" int getpalookup_replace(int a1, int a2)
{
    if (gFogMode)
        return ClipHigh(a1>>8, 15)*16+ClipRange(a2>>2, 0, 15);
    else
        return ClipRange((a1>>8)+a2, 0, 63);
}

extern "C"
{
    void loadvoxel_replace(int nVoxel);
    void initspritelists_replace(void);
    int insertsprite_replace(short a1, short a2);
    int deletesprite_replace(short a1);
    int changespritesect_replace(short nSprite, short nSector);
    int changespritestat_replace(short nSprite, short nStatus);


    extern void* (*kmalloc)(size_t);
    extern void (*kfree)(void*);
    extern void (*loadvoxel)(int32_t);
    extern void (*initcache)(intptr_t, int32_t);
    extern void (*allocache)(intptr_t*, int32_t, char*);
    extern void (*suckcache)(int32_t*);
    extern void (*agecache)();
    extern void (*initspritelists)();
    extern int32_t(*insertsprite)(short, short);
    extern int32_t(*deletesprite)(short);
    extern int32_t(*changespritesect)(short, short);
    extern int32_t(*changespritestat)(short, short);
    extern int32_t(*loadpics)(char*);
    extern void (*loadtile)(short);
    extern intptr_t(*allocatepermanenttile)(short, int32_t, int32_t);
    extern void (*uninitengine)();
    extern void (*loadpalette)();
    extern int32_t(*getpalookup)(int32_t, int32_t);
}

void replace_hook()
{
    kmalloc = kmalloc_replace;
    kfree = kfree_replace;
    loadvoxel = loadvoxel_replace;
    initcache = initcache_replace;
    allocache = allocache_replace;
    agecache = agecache_replace;
    initspritelists = initspritelists_replace;
    insertsprite = insertsprite_replace;
    deletesprite = deletesprite_replace;
    changespritesect = changespritesect_replace;
    changespritestat = changespritestat_replace;
    loadpics = loadpics_replace;
    loadtile = loadtile_replace;
    allocatepermanenttile = allocatepermanenttile_replace;
    animateoffs = animateoffs_replace;
    uninitengine = uninitengine_replace;
    loadpalette = loadpalette_replace;
    getpalookup = getpalookup_replace;
}
