// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.
// This file has been modified from Ken Silverman's original release
#pragma once

#define MAXSECTORS 1024
#define MAXWALLS 8192
#define MAXSPRITES 4096

#define MAXTILES 6144
#define MAXSTATUS 1024
#define MAXPLAYERS 16
#define MAXXDIM 1600
#define MAXYDIM 1200
#define MAXPALOOKUPS 256
#define MAXPSKYTILES 256
#define MAXSPRITESONSCREEN 1024

#define CLIPMASK0 (((1L)<<16)+1L)
#define CLIPMASK1 (((256L)<<16)+64L)

	//Make all variables in BUILD.H defined in the ENGINE,
	//and externed in GAME
#ifdef ENGINE
	#define EXTERN
#else
	#define EXTERN extern
#endif

#pragma pack(push,1)

//ceilingstat/floorstat:
//   bit 0: 1 = parallaxing, 0 = not                                 "P"
//   bit 1: 1 = groudraw, 0 = not
//   bit 2: 1 = swap x&y, 0 = not                                    "F"
//   bit 3: 1 = double smooshiness                                   "E"
//   bit 4: 1 = x-flip                                               "F"
//   bit 5: 1 = y-flip                                               "F"
//   bit 6: 1 = Align texture to first wall of sector                "R"
//   bits 7-8:                                                       "T"
//          00 = normal floors
//          01 = masked floors
//          10 = transluscent masked floors
//          11 = reverse transluscent masked floors
//   bits 9-15: reserved

	//40 bytes
typedef struct
{
	short wallptr, wallnum;
	int32_t ceilingz, floorz;
	short ceilingstat, floorstat;
	short ceilingpicnum, ceilingheinum;
	signed char ceilingshade;
	char ceilingpal, ceilingxpanning, ceilingypanning;
	short floorpicnum, floorheinum;
	signed char floorshade;
	char floorpal, floorxpanning, floorypanning;
	char visibility, filler;
	short lotag, hitag, extra;
} sectortype;

//cstat:
//   bit 0: 1 = Blocking wall (use with clipmove, getzrange)         "B"
//   bit 1: 1 = bottoms of invisible walls swapped, 0 = not          "2"
//   bit 2: 1 = align picture on bottom (for doors), 0 = top         "O"
//   bit 3: 1 = x-flipped, 0 = normal                                "F"
//   bit 4: 1 = masking wall, 0 = not                                "M"
//   bit 5: 1 = 1-way wall, 0 = not                                  "1"
//   bit 6: 1 = Blocking wall (use with hitscan / cliptype 1)        "H"
//   bit 7: 1 = Transluscence, 0 = not                               "T"
//   bit 8: 1 = y-flipped, 0 = normal                                "F"
//   bit 9: 1 = Transluscence reversing, 0 = normal                  "T"
//   bits 10-15: reserved

	//32 bytes
typedef struct
{
	int32_t x, y;
	short point2, nextwall, nextsector, cstat;
	short picnum, overpicnum;
	signed char shade;
	char pal, xrepeat, yrepeat, xpanning, ypanning;
	short lotag, hitag, extra;
} walltype;

//cstat:
//   bit 0: 1 = Blocking sprite (use with clipmove, getzrange)       "B"
//   bit 1: 1 = transluscence, 0 = normal                            "T"
//   bit 2: 1 = x-flipped, 0 = normal                                "F"
//   bit 3: 1 = y-flipped, 0 = normal                                "F"
//   bits 5-4: 00 = FACE sprite (default)                            "R"
//             01 = WALL sprite (like masked walls)
//             10 = FLOOR sprite (parallel to ceilings&floors)
//   bit 6: 1 = 1-sided sprite, 0 = normal                           "1"
//   bit 7: 1 = Real centered centering, 0 = foot center             "C"
//   bit 8: 1 = Blocking sprite (use with hitscan / cliptype 1)      "H"
//   bit 9: 1 = Transluscence reversing, 0 = normal                  "T"
//   bits 10-14: reserved
//   bit 15: 1 = Invisible sprite, 0 = not invisible

	//44 bytes
typedef struct
{
	int32_t x, y, z;
	short cstat, picnum;
	signed char shade;
	char pal, clipdist, filler;
	unsigned char xrepeat, yrepeat;
	signed char xoffset, yoffset;
	short sectnum, statnum;
	short ang, owner, xvel, yvel, zvel;
	short lotag, hitag, extra;
} spritetype;

#pragma pack(pop)

EXTERN sectortype sector[MAXSECTORS];
EXTERN walltype wall[MAXWALLS];
EXTERN spritetype sprite[MAXSPRITES];

EXTERN int32_t spritesortcnt;
EXTERN spritetype tsprite[MAXSPRITESONSCREEN];

EXTERN char vidoption;
EXTERN int32_t xdim, ydim, ylookup[MAXYDIM+1], numpages;
EXTERN int32_t yxaspect, viewingrange;

EXTERN int32_t validmodecnt;
EXTERN short validmode[256];
EXTERN int32_t validmodexdim[256], validmodeydim[256];

EXTERN short numsectors, numwalls;
//EXTERN volatile int32_t totalclock;
int32_t totalclock_get();
void totalclock_set(int32_t value);
void totalclock_add(int32_t value);
void totalclock_sub(int32_t value);
#define totalclock totalclock_get()

EXTERN int32_t numframes, randomseed;
EXTERN short sintable[2048];
EXTERN char palette[768];
EXTERN short numpalookups;
EXTERN char *palookup[MAXPALOOKUPS];
EXTERN char parallaxtype, showinvisibility;
EXTERN int32_t parallaxyoffs, parallaxyscale;
EXTERN int32_t visibility, parallaxvisibility;

EXTERN int32_t windowx1, windowy1, windowx2, windowy2;
EXTERN short startumost[MAXXDIM], startdmost[MAXXDIM];

EXTERN short pskyoff[MAXPSKYTILES], pskybits;

EXTERN short headspritesect[MAXSECTORS+1], headspritestat[MAXSTATUS+1];
EXTERN short prevspritesect[MAXSPRITES], prevspritestat[MAXSPRITES];
EXTERN short nextspritesect[MAXSPRITES], nextspritestat[MAXSPRITES];

EXTERN short tilesizx[MAXTILES], tilesizy[MAXTILES];
EXTERN char walock[MAXTILES];
EXTERN int32_t numtiles, picanm[MAXTILES];
EXTERN intptr_t waloff[MAXTILES];

	//These variables are for auto-mapping with the draw2dscreen function.
	//When you load a new board, these bits are all set to 0 - since
	//you haven't mapped out anything yet.  Note that these arrays are
	//bit-mapped.
	//If you want draw2dscreen() to show sprite #54 then you say:
	//   spritenum = 54;
	//   show2dsprite[spritenum>>3] |= (1<<(spritenum&7));
	//And if you want draw2dscreen() to not show sprite #54 then you say:
	//   spritenum = 54;
	//   show2dsprite[spritenum>>3] &= ~(1<<(spritenum&7));
	//Automapping defaults to 0 (do nothing).  If you set automapping to 1,
	//   then in 3D mode, the walls and sprites that you see will show up the
	//   next time you flip to 2D mode.

EXTERN char show2dsector[(MAXSECTORS+7)>>3];
EXTERN char show2dwall[(MAXWALLS+7)>>3];
EXTERN char show2dsprite[(MAXSPRITES+7)>>3];
EXTERN char automapping;

EXTERN char gotpic[(MAXTILES+7)>>3];
EXTERN char gotsector[(MAXSECTORS+7)>>3];

/*************************************************************************
POSITION VARIABLES:

		POSX is your x - position ranging from 0 to 65535
		POSY is your y - position ranging from 0 to 65535
			(the length of a side of the grid in EDITBORD would be 1024)
		POSZ is your z - position (height) ranging from 0 to 65535, 0 highest.
		ANG is your angle ranging from 0 to 2047.  Instead of 360 degrees, or
			 2 * PI radians, I use 2048 different angles, so 90 degrees would
			 be 512 in my system.

SPRITE VARIABLES:

	EXTERN short headspritesect[MAXSECTORS+1], headspritestat[MAXSTATUS+1];
	EXTERN short prevspritesect[MAXSPRITES], prevspritestat[MAXSPRITES];
	EXTERN short nextspritesect[MAXSPRITES], nextspritestat[MAXSPRITES];

	Example: if the linked lists look like the following:
		 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
		 ³      Sector lists:               Status lists:               ³
		 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
		 ³  Sector0:  4, 5, 8             Status0:  2, 0, 8             ³
		 ³  Sector1:  16, 2, 0, 7         Status1:  4, 5, 16, 7, 3, 9   ³
		 ³  Sector2:  3, 9                                              ³
		 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
	Notice that each number listed above is shown exactly once on both the
		left and right side.  This is because any sprite that exists must
		be in some sector, and must have some kind of status that you define.


Coding example #1:
	To go through all the sprites in sector 1, the code can look like this:

		sectnum = 1;
		i = headspritesect[sectnum];
		while (i != -1)
		{
			nexti = nextspritesect[i];

			//your code goes here
			//ex: printf("Sprite %d is in sector %d\n",i,sectnum);

			i = nexti;
		}

Coding example #2:
	To go through all sprites with status = 1, the code can look like this:

		statnum = 1;        //status 1
		i = headspritestat[statnum];
		while (i != -1)
		{
			nexti = nextspritestat[i];

			//your code goes here
			//ex: printf("Sprite %d has a status of 1 (active)\n",i,statnum);

			i = nexti;
		}

			 insertsprite(short sectnum, short statnum);
			 deletesprite(short spritenum);
			 changespritesect(short spritenum, short newsectnum);
			 changespritestat(short spritenum, short newstatnum);

TILE VARIABLES:
		NUMTILES - the number of tiles found TILES.DAT.
		TILESIZX[MAXTILES] - simply the x-dimension of the tile number.
		TILESIZY[MAXTILES] - simply the y-dimension of the tile number.
		WALOFF[MAXTILES] - the actual 32-bit offset pointing to the top-left
								 corner of the tile.
		PICANM[MAXTILES] - flags for animating the tile.

TIMING VARIABLES:
		TOTALCLOCK - When the engine is initialized, TOTALCLOCK is set to zero.
			From then on, it is incremented 120 times a second by 1.  That
			means that the number of seconds elapsed is totalclock / 120.
		NUMFRAMES - The number of times the draw3dscreen function was called
			since the engine was initialized.  This helps to determine frame
			rate.  (Frame rate = numframes * 120 / totalclock.)

OTHER VARIABLES:

		STARTUMOST[320] is an array of the highest y-coordinates on each column
				that my engine is allowed to write to.  You need to set it only
				once.
		STARTDMOST[320] is an array of the lowest y-coordinates on each column
				that my engine is allowed to write to.  You need to set it only
				once.
		SINTABLE[2048] is a sin table with 2048 angles rather than the
			normal 360 angles for higher precision.  Also since SINTABLE is in
			all integers, the range is multiplied by 16383, so instead of the
			normal -1<sin(x)<1, the range of sintable is -16383<sintable[]<16383
			If you use this sintable, you can possibly speed up your code as
			well as save space in memory.  If you plan to use sintable, 2
			identities you may want to keep in mind are:
				sintable[ang&2047]       = sin(ang * (3.141592/1024)) * 16383
				sintable[(ang+512)&2047] = cos(ang * (3.141592/1024)) * 16383
		NUMSECTORS - the total number of existing sectors.  Modified every time
			you call the loadboard function.
***************************************************************************/

extern int engine_revision;


void initengine();
void faketimerhandler();
int32_t initmouse();
void getmousevalues(short *mousx, short *mousy, short *bstatus);
int32_t loadpics(char *filename);
void qloadkvx(int32_t voxindex, char *filename);
int32_t allocatepermanenttile(short tilenume, int32_t xsiz, int32_t ysiz);
void copytilepiece(int32_t tilenume1, int32_t sx1, int32_t sy1, int32_t xsiz, int32_t ysiz,
				  int32_t tilenume2, int32_t sx2, int32_t sy2);
void makepalookup(int32_t palnum, char *remapbuf, signed char r, signed char g, signed char b, char dastat);
void printext256(int32_t xpos, int32_t ypos, short col, short backcol, char name[82], char fontsize);
void nextpage();
void uninitengine();
void setvmode(int a);
int32_t nextsectorneighborz(short sectnum, int32_t thez, short topbottom, short direction);
int32_t krand();
int32_t loadboard(char *filename, int32_t *daposx, int32_t *daposy, int32_t *daposz,
			 short *daang, short *dacursectnum);
int32_t lastwall(short point);
int32_t insertsprite(short sectnum, short statnum);
int32_t deletesprite(short spritenum);
int32_t changespritesect(short spritenum, short newsectnum);
int32_t changespritestat(short spritenum, short newstatnum);
void setview(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
int32_t hitscan(int32_t xs, int32_t ys, int32_t zs, short sectnum, int32_t vx, int32_t vy, int32_t vz,
	short *hitsect, short *hitwall, short *hitsprite,
	int32_t *hitx, int32_t *hity, int32_t *hitz, uint32_t cliptype);
void dragpoint(short pointhighlight, int32_t dax, int32_t day);
int32_t setsprite(short spritenum, int32_t newx, int32_t newy, int32_t newz);
void rotatepoint(int32_t xpivot, int32_t ypivot, int32_t x, int32_t y, short daang, int32_t *x2, int32_t *y2);
int32_t clipinsidebox(int32_t x, int32_t y, short wallnum, int32_t walldist);
int32_t cansee(int32_t x1, int32_t y1, int32_t z1, short sect1, int32_t x2, int32_t y2, int32_t z2, short sect2);
int32_t ksqrt(int32_t num);
extern int32_t clipmoveboxtracenum;
int32_t clipmove (int32_t *x, int32_t *y, int32_t *z, short *sectnum,
			 int32_t xvect, int32_t yvect,
			 int32_t walldist, int32_t ceildist, int32_t flordist, uint32_t cliptype);
int32_t getangle(int32_t xvect, int32_t yvect);
int32_t getzsofslope(short sectnum, int32_t dax, int32_t day, int32_t *ceilz, int32_t *florz);
int32_t pushmove (int32_t *x, int32_t *y, int32_t *z, short *sectnum,
			 int32_t walldist, int32_t ceildist, int32_t flordist, uint32_t cliptype);
int32_t getceilzofslope(short sectnum, int32_t dax, int32_t day);
int32_t getflorzofslope(short sectnum, int32_t dax, int32_t day);
int32_t neartag(int32_t xs, int32_t ys, int32_t zs, short sectnum, short ange, short *neartagsector, short *neartagwall, short *neartagsprite, int32_t *neartaghitdist, int32_t neartagrange, char tagsearch);
int32_t inside(int32_t x, int32_t y, short sectnum);
int32_t getzrange(int32_t x, int32_t y, int32_t z, short sectnum,
			 int32_t *ceilz, int32_t *ceilhit, int32_t *florz, int32_t *florhit,
			 int32_t walldist, uint32_t cliptype);
void flushperms();
void rotatesprite(int32_t sx, int32_t sy, int32_t z, short a, short picnum, signed char dashade, char dapalnum, char dastat, int32_t cx1, int32_t cy1, int32_t cx2, int32_t cy2);
void drawrooms(int32_t daposx, int32_t daposy, int32_t daposz,
			 short daang, int32_t dahoriz, short dacursectnum);
void drawmasks();
void setviewtotile(short tilenume, int32_t xsiz, int32_t ysiz);
void setaspect(int32_t daxrange, int32_t daaspect);
void preparemirror(int32_t dax, int32_t day, int32_t daz, short daang, int32_t dahoriz, short dawall, short dasector, int32_t *tposx, int32_t *tposy, short *tang);
void completemirror();
void setviewback();
void clearview(int32_t dacol);
void drawmapview(int32_t dax, int32_t day, int32_t zoome, short ang);
int32_t screencapture(char* filename, char inverseit);
void getvalidvesamodes();
int32_t setgamemode(char davidoption, int32_t daxdim, int32_t daydim);
void printscreeninterrupt();
void setbrightness(char dabrightness, char *dapal);
void clearallviews(int32_t dacol);
int VBE_setPalette(int32_t start, int32_t num, char *dapal);
void updatesector(int32_t x, int32_t y, short* sectnum);
void loadtile(short tilenume);
void drawline256(int32_t x1, int32_t y1, int32_t x2, int32_t y2, char col);
void *kmalloc(size_t size);
void kfree(void *buffer);
void alignflorslope(short dasect, int32_t x, int32_t y, int32_t z);
