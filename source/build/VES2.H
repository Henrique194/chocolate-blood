// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.
// This file has been modified from Ken Silverman's original release

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compat.h"

static char *screen = NULL, vesachecked = 0;
int32_t xres, yres, bytesperline, imageSize, maxpages;
intptr_t frameplace;
int32_t buffermode, origbuffermode;
char permanentupdate = 0;
intptr_t activepagelookup[64];
static int32_t ves2lastx[MAXYDIM];
intptr_t globlinplace;

typedef struct {
	int xdim, ydim;
} vidmode_t;

vidmode_t modes[] = {
	{ 320, 200 },
	{ 640, 480 },
	{ 800, 600 },
	{ 1024, 768 },
	{ 1280, 1024 },
	{ 1600, 1200 },
	{ -1, -1 }
};

static inline void qlimitrate()
{
}

void getvalidvesamodes()
{
	int32_t i, j, k;
	short *p, *p2;

	if (vesachecked) return;
	vesachecked = 1;

	validmodecnt = 0;

	for (i = 0; modes[i].xdim != -1; i++)
	{
		validmode[validmodecnt] = i;
		validmodexdim[validmodecnt] = modes[i].xdim;
		validmodeydim[validmodecnt] = modes[i].ydim;
		validmodecnt++;
	}

	for(i=1;i<validmodecnt;i++)
		for(j=0;j<i;j++)
			if (validmodeydim[i] < validmodeydim[j])
			{
				k = validmode[i]; validmode[i] = validmode[j]; validmode[j] = k;
				k = validmodexdim[i]; validmodexdim[i] = validmodexdim[j]; validmodexdim[j] = k;
				k = validmodeydim[i]; validmodeydim[i] = validmodeydim[j]; validmodeydim[j] = k;
			}

	for(i=1;i<validmodecnt;i++)
		for(j=0;j<i;j++)
			if (validmodexdim[i] < validmodexdim[j])
			{
				k = validmode[i]; validmode[i] = validmode[j]; validmode[j] = k;
				k = validmodexdim[i]; validmodexdim[i] = validmodexdim[j]; validmodexdim[j] = k;
				k = validmodeydim[i]; validmodeydim[i] = validmodeydim[j]; validmodeydim[j] = k;
			}
}

int setvesa(int32_t x, int32_t y)
{
	div_t dived;
	int32_t i, j, k;
	short *p, *p1, *p2;

	getvalidvesamodes();

	xres = x; yres = y;
	for(k=0;k<validmodecnt;k++)
	{
		int id = validmode[k];
		if (modes[id].xdim != x) continue;
		if (modes[id].ydim != y) continue;

		Video_Set(1, xres, yres);

		bytesperline = video_row_stride;
		maxpages = video_pages;

		globlinplace = (intptr_t)video_buffer;

		//Linear mode
		buffermode = (maxpages<=1);
		imageSize = bytesperline*yres;
		frameplace = globlinplace;
		j = 0;
		for(i=0;i<maxpages;i++)
		{
			activepagelookup[i] = globlinplace+j;
			j += imageSize;
		}

		origbuffermode = buffermode;
		return(0);
	}
	return(-1);
}

void setactivepage(int32_t dapagenum)
{
	frameplace = activepagelookup[dapagenum];
}

void setvisualpage(int32_t dapagenum)
{
	int32_t i, j, p, dx, cx1, cy1, cx2, cy2, delta, x, y, y1, y2, ny1, ny2;

	if (buffermode != 0)
	{
		cx1 = windowx1; cy1 = windowy1; cx2 = windowx2; cy2 = windowy2;
		if (permanentupdate) { cx1 = 0; cy1 = 0; cx2 = xdim-1; cy2 = ydim-1; }
		dx = cx2-cx1+1;

		// linear

		p = FP_OFF(screen);
		delta = activepagelookup[dapagenum&0x7fffffff]-FP_OFF(screen);
		if (permanentupdate == 0)
		{
			y1 = startumost[cx1]; y2 = y1;
			for(x=cx1;x<=cx2;x++)
			{
				ny1 = startumost[x]-1; ny2 = startdmost[x];
				if (ny1 < ny2-1)
				{
					if (ny1 >= y2)
					{
						while (y1 < y2-1)
						{
							y1++; if ((y1&31) == 0) faketimerhandler();
								//x,y1
							i = p+ylookup[y1]+ves2lastx[y1];
							copybufbyte(i,i+delta,x-ves2lastx[y1]);
						}
						y1 = ny1;
					}
					else
					{
						while (y1 < ny1)
						{
							y1++; if ((y1&31) == 0) faketimerhandler();
								//x-1,y1
							i = p+ylookup[y1]+ves2lastx[y1];
							copybufbyte(i,i+delta,x-ves2lastx[y1]);
						}
						while (y1 > ny1) ves2lastx[y1--] = x;
					}
					while (y2 > ny2)
					{
						y2--; if ((y2&31) == 0) faketimerhandler();
							//x-1,y2
						i = p+ylookup[y2]+ves2lastx[y2];
						copybufbyte(i,i+delta,x-ves2lastx[y2]);
					}
					while (y2 < ny2) ves2lastx[y2++] = x;
				}
				else
				{
					while (y1 < y2-1)
					{
						y1++; if ((y1&31) == 0) faketimerhandler();
							//x-1,y1
						i = p+ylookup[y1]+ves2lastx[y1];
						copybufbyte(i,i+delta,x-ves2lastx[y1]);
					}
					if (x == cx2) break;
					y1 = startumost[x+1]; y2 = y1;
				}
			}
			while (y1 < y2-1)
			{
				y1++; if ((y1&31) == 0) faketimerhandler();
					//cx2+1,y1
				i = p+ylookup[y1]+ves2lastx[y1];
				copybufbyte(i,i+delta,cx2+1-ves2lastx[y1]);
			}
		}
		else
		{
			p += ylookup[cy1]+cx1;
			delta = activepagelookup[dapagenum&0x7fffffff]-FP_OFF(screen);
			for(y=cy1;y<=cy2;y++)
			{
				copybufbyte(p,p+delta,dx);
				p += ylookup[1];
				if ((y&31) == 0) faketimerhandler();
			}
		}
		Video_BlitPage(0);
		permanentupdate = 0;
	}
	if (origbuffermode == 0)
	{
		Video_BlitPage(dapagenum & 0x7fffffff);
		if (dapagenum >= 0) { qlimitrate(); faketimerhandler(); }
	}
}

void uninitvesa()
{
	vesachecked = 0;
}

int VBE_setPalette(int32_t start, int32_t num, char *dapal)
{
	int32_t i, j, k;

	j = 0; k = (start<<2);
	for(i=0;i<num;i++)
	{
		video_palette[j] = dapal[k+2];
		video_palette[j+1] = dapal[k+1];
		video_palette[j+2] = dapal[k];
		j += 3; k += 4;
	}
	return(1);
}

int VBE_getPalette(int32_t start, int32_t num, char *dapal)
{
	int32_t i;

	for(i=num;i>0;i--)
	{
		dapal[2] = video_palette[start++];
		dapal[1] = video_palette[start++];
		dapal[0] = video_palette[start++];
		dapal += 4;
	}
	return(1);
}
