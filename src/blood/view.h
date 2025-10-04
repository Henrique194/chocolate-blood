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
#ifndef _VIEW_H_
#define _VIEW_H_

#include "typedefs.h"
#include "engine.h"
#include "config.h"
#include "controls.h"
#include "misc.h"

struct FONT {
    int tile;
    int xSize;
    int ySize;
    int space;
};

extern FONT gFont[];

extern int gViewMode;
extern int gZoom;
extern int gViewX0S;
extern int gViewX1S;
extern int gViewY0S;
extern int gViewY1S;
extern int gViewIndex;

extern int gShowFrameRate;

extern int32_t gScreenTilt;
extern int deliriumTilt;
extern int deliriumPitch;
extern int deliriumTurn;

enum VIEWPOS : int32_t {
    VIEWPOS_0,
    VIEWPOS_1,
    VIEWPOS_2,
};

extern VIEWPOS gViewPos;

enum INTERPOLATE_TYPE : int32_t {
    INTERPOLATE_TYPE_INT = 0,
    INTERPOLATE_TYPE_SHORT,
};

struct LOCATION {
    int x, y, z;
    int ang;
};

extern LOCATION gPrevSpriteLoc[kMaxSprites];

extern byte gInterpolateSprite[(kMaxSprites+7)>>3];
extern byte gInterpolateWall[(kMaxWalls+7)>>3];
extern byte gInterpolateSector[(kMaxSectors+7)>>3];

void viewAddInterpolation(void *, INTERPOLATE_TYPE);

inline void viewInterpolateSector(int nSector, SECTOR *pSector)
{
    if (!TestBitString(gInterpolateSector, nSector))
    {
        viewAddInterpolation(&pSector->floorz, INTERPOLATE_TYPE_INT);
        viewAddInterpolation(&pSector->ceilingz, INTERPOLATE_TYPE_INT);
        viewAddInterpolation(&pSector->floorheinum, INTERPOLATE_TYPE_SHORT);
        SetBitString(gInterpolateSector, nSector);
    }
}

inline void viewInterpolateWall(int nWall, WALL *pWall)
{
    if (!TestBitString(gInterpolateWall, nWall))
    {
        viewAddInterpolation(&pWall->x, INTERPOLATE_TYPE_INT);
        viewAddInterpolation(&pWall->y, INTERPOLATE_TYPE_INT);
        SetBitString(gInterpolateWall, nWall);
    }
}

inline void viewBackupSpriteLoc(int nSprite, SPRITE *pSprite)
{
    if (!TestBitString(gInterpolateSprite, nSprite))
    {
        LOCATION *pPrevLoc = &gPrevSpriteLoc[nSprite];
        pPrevLoc->x = pSprite->x;
        pPrevLoc->y = pSprite->y;
        pPrevLoc->z = pSprite->z;
        pPrevLoc->ang = pSprite->ang;
        SetBitString(gInterpolateSprite, nSprite);
    }
}

void func_1EC78(int, const char *, const char *, const char *);
void viewResizeView(int);
void viewToggle(int);
void viewSetMessage(const char *);
void viewDrawText(int, const char *, int, int, int, int, int position = 0, QBOOL shadow = 0);
void viewGetFontInfo(int nFont, const char *pString, int *pXSize, int *pYSize);
void viewUpdatePages(void);
void viewDrawSprite(int32_t,int32_t,int32_t,int,int,schar,byte,ushort,int32_t,int32_t,int32_t,int32_t);
void viewInit(void);
void viewBackupView(int);
void viewInitializePrediction(void);
void viewProcessSprites(int cX, int cY, int cZ);
void viewSetErrorMessage(const char *);
void viewDrawScreen(void);
void viewClearInterpolations(void);
void viewCorrectPrediction(void);
void viewUpdatePrediction(QINPUT *);

#endif // !_VIEW_H_
