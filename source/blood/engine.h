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
#ifndef _BUILD_H_
#define _BUILD_H_

#include "typedefs.h"

#define kMaxVoxels 512
#define kMaxVoxMips 5
#define kMaxSectors 1024
#define kMaxWalls 8192
#define kMaxSprites 4096
#define kMaxTiles 6144
#define kMaxStatus 1024
#define kMaxSkyTiles 256
#define kMaxPlayers 8
#define kMaxViewSprites 1024

#pragma pack(push, 1)

struct SECTOR {
    short wallptr, wallnum;
    int32_t ceilingz, floorz;
    ushort ceilingstat, floorstat;
    short ceilingpicnum, ceilingheinum;
    signed char ceilingshade;
    byte ceilingpal, ceilingxpanning, ceilingypanning;
    short floorpicnum, floorheinum;
    signed char floorshade;
    byte floorpal, floorxpanning, floorypanning;
    byte visibility, align;
    ushort type;
    short hitag, extra;
};

enum {
    kSectorStat0 = 1,
    kSectorStat1 = 2,
    kSectorStat3 = 8,
    kSectorStat6 = 64,
    kSectorStat31 = 32768
};

struct WALL
{
    int32_t x, y;
    short point2, nextwall, nextsector;
    ushort cstat;
    short picnum, overpicnum;
    signed char shade;
    byte pal, xrepeat, yrepeat, xpanning, ypanning;
    ushort type;
    short hitag, extra;
};

enum {
    kWallStat2 = 4,
    kWallStat3 = 8,
    kWallStat4 = 16,
    kWallStat5 = 32,
    kWallStat6 = 64,
    kWallStat8 = 256,

    kWallStat14 = 16384,
    kWallStat15 = 32768,

    kWallStat14_15 = 49152,
};

struct SPRITE
{
    int32_t x, y, z;
    ushort cstat;
    short picnum;
    signed char shade;
    byte pal, clipdist, detail;
    byte xrepeat, yrepeat;
    signed char xoffset, yoffset;
    short sectnum, statnum;
    short ang, owner, index, yvel, inittype;
    short type;
    ushort flags;
    short extra;
};

enum {
    kSpriteStat0 = 1,
    kSpriteStat3 = 8,
    kSpriteStat5 = 32,
    kSpriteStat7 = 128,
    kSpriteStat8 = 256,
    kSpriteStat13 = 8192,
    kSpriteStat14 = 16384,
    kSpriteStat15 = 32768,
    kSpriteStat31 = 32768, // doh
    kSpriteStat13_14 = 24576,
};

enum {
    kSpriteFace = 0,
    kSpriteWall = 0x10,
    kSpriteFloor = 0x20,
    kSpriteVoxel = 0x30,
    kSpriteMask = 0x30
};

struct PICANM {
    unsigned int animframes : 5;
    unsigned int at0_5 : 1;
    unsigned int animtype : 2;
    signed int xoffset : 8;
    signed int yoffset : 8;
    unsigned int animspeed : 4;
    unsigned int at3_4 : 3; // type
    unsigned int at3_7 : 1; // filler
};

#pragma pack(pop)

extern "C" {
    
int32_t totalclock_get();
void totalclock_set(int32_t value);
void totalclock_add(int32_t value);
void totalclock_sub(int32_t value);
#define totalclock totalclock_get()
extern byte palette[768];
extern byte *palookup[256];
extern int parallaxvisibility;
extern ushort numpalookups;
extern QBOOL paletteloaded;
extern byte *transluc;
extern byte vidoption;

extern int windowx1, windowy1, windowx2, windowy2;

extern int xdim, ydim;
extern byte *frameplace;
extern int ylookup[1201];

extern SECTOR sector[kMaxSectors];
extern WALL wall[kMaxWalls];
extern SPRITE sprite[kMaxSprites];

extern short headspritesect[kMaxSectors+1], headspritestat[kMaxStatus+1];
extern short prevspritesect[kMaxSprites], prevspritestat[kMaxSprites];
extern short nextspritesect[kMaxSprites], nextspritestat[kMaxSprites];

extern short numsectors, numwalls;

extern byte show2dsector[(kMaxSectors+7)>>3];
extern byte show2dwall[(kMaxWalls+7)>>3];
extern byte show2dsprite[(kMaxSprites+7)>>3];

extern short pskyoff[kMaxSkyTiles], pskybits;

extern int visibility;

extern byte parallaxtype;
extern byte picsiz[kMaxTiles];

extern byte *voxoff[kMaxVoxels][kMaxVoxMips];

extern short tilesizx[kMaxTiles];
extern short tilesizy[kMaxTiles];
extern PICANM picanm[kMaxTiles];

extern int artversion;
extern int numtiles;

extern byte tilefilenum[kMaxTiles];
extern int tilefileoffs[kMaxTiles];
extern byte gotpic[(kMaxTiles+7)>>3];

extern byte *waloff[kMaxTiles];

void fixtransluscence(intptr_t);
void initengine(void);
extern void (*uninitengine)(void);
int32_t setgamemode(char davidoption, int32_t daxdim, int32_t daydim);
void clearview(int32_t dacol);
void nextpage(void);
void setview(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
extern int (*getpalookup)(int, int);

extern void (*initspritelists)(void);
extern int (*insertsprite)(short, short);
extern int (*deletesprite)(short);
extern int (*changespritesect)(short, short);
extern int (*changespritestat)(short, short);

void printext256(int32_t xpos, int32_t ypos, short col, short backcol, char name[82], char fontsize);

extern int (*animateoffs)(short a1, ushort a2);

void rotatesprite(int32_t sx, int32_t sy, int32_t z, short a, short picnum, signed char dashade, char dapalnum, char dastat, int32_t cx1, int32_t cy1, int32_t cx2, int32_t cy2);

void setviewtotile(short tilenume, int32_t xsiz, int32_t ysiz);

void setviewback();

void setaspect(int32_t daxrange, int32_t daaspect);

void drawrooms(int32_t daposx, int32_t daposy, int32_t daposz,
			 short daang, int32_t dahoriz, short dacursectnum);

void drawmasks(void);

extern byte gotsector[(kMaxSectors+7)>>3];

enum {
    kRSStat0 = 1,
    kRSStat2 = 4,
    kRSStat8 = 256,
};

int32_t ksqrt(int32_t num);

int32_t getangle(int32_t xvect, int32_t yvect);

int32_t inside(int32_t x, int32_t y, short sectnum);
int32_t getzsofslope(short sectnum, int32_t dax, int32_t day, int32_t* ceilz, int32_t* florz);
int32_t getflorzofslope(short sectnum, int32_t dax, int32_t day);
int32_t getceilzofslope(short sectnum, int32_t dax, int32_t day);

int32_t cansee(int32_t x1, int32_t y1, int32_t z1, short sect1, int32_t x2, int32_t y2, int32_t z2, short sect2);

#define CLIPMASK0 (((1L)<<16)+1L)
#define CLIPMASK1 (((256L)<<16)+64L)

extern int hitscangoalx, hitscangoaly;

int32_t hitscan(int32_t xs, int32_t ys, int32_t zs, short sectnum, int32_t vx, int32_t vy, int32_t vz,
	short *hitsect, short *hitwall, short *hitsprite,
	int32_t *hitx, int32_t *hity, int32_t *hitz, uint32_t cliptype);

void getzrange(int32_t x, int32_t y, int32_t z, short sectnum,
    int32_t* ceilz, int32_t* ceilhit, int32_t* florz, int32_t* florhit,
    int32_t walldist, uint32_t cliptype);

int32_t clipmove (int32_t *x, int32_t *y, int32_t *z, short *sectnum,
			 int32_t xvect, int32_t yvect,
			 int32_t walldist, int32_t ceildist, int32_t flordist, uint32_t cliptype);

int32_t pushmove (int32_t *x, int32_t *y, int32_t *z, short *sectnum,
			 int32_t walldist, int32_t ceildist, int32_t flordist, uint32_t cliptype);

void updatesector(int32_t x, int32_t y, short* sectnum);

int32_t setsprite(short spritenum, int32_t newx, int32_t newy, int32_t newz);

void getvalidvesamodes(void);

extern short validmode[];
extern int32_t validmodexdim[];
extern int32_t validmodeydim[];
extern int32_t validmodecnt;

extern int numpages;

extern int yxaspect;

extern int spritesortcnt;
extern SPRITE tsprite[];

extern short sintable[];

extern int clipmoveboxtracenum;

extern byte automapping;

extern int randomseed;

extern byte showinvisibility;

extern int parallaxyoffs;
extern int parallaxyscale;

void drawline256(int32_t x1, int32_t y1, int32_t x2, int32_t y2, char col);
void drawmapview(int32_t dax, int32_t day, int32_t zoome, short ang);

int32_t lastwall(short point);
void alignflorslope(short dasect, int32_t x, int32_t y, int32_t z);
void alignceilslope(short dasect, int32_t x, int32_t y, int32_t z);
int32_t sectorofwall(short theline);

void preparemirror(int32_t dax, int32_t day, int32_t daz, short daang, int32_t dahoriz, short dawall, short dasector, int32_t* tposx, int32_t* tposy, short* tang);
void completemirror();

extern int32_t startposx;
extern int32_t startposy;
extern int32_t startposz;
extern short startsectnum;
extern short startang;

void screencapture(char *, char);

void faketimerhandler(void);

int krand(void);

// mmulti.c

extern short numplayers;
extern short myconnectindex;
extern short connecthead;
extern short connectpoint2[];
extern byte syncstate;

void sendpacket(int, char *, int);
short getpacket(short *, char *);
void initmultiplayers(int, int, int);
void setpackettimeout(int, int);
void sendlogoff(void);
void uninitmultiplayers(void);

// a.asm

extern byte *palookupoffse[4];
extern byte *bufplce[4];
extern int vince[4];
extern int vplce[4];

void setupvlineasm(int);
int vlineasm4(int, byte*);
int vlineasm1(int, byte*, int, int, byte*, byte*);
int mvlineasm1(int, byte*, int, int, byte*, byte*);
int mvlineasm4(int, byte*);


}

#endif // !_BUILD_H_
