#include <stdio.h>
#include <stdlib.h>
#include "compat.h"
#include "types.h"
#include "develop.h"
#include "fixed.h"
#include "angles.h"

#define SWAP(a,b) \
{ \
a = (a) ^ (b); \
b = (a) ^ (b); \
a = (a) ^ (b); \
} \


int32 FindDistance2D(int32 dx, int32 dy)
{
	int t;

	dx = abs(dx);
	dy = abs(dy);
	if (!dy)
		return dx;
	if (!dx)
		return dy;

	if (dx < dy)
		SWAP(dx, dy);

	t = dy + (dy >> 1);
	return dx - (dx >> 5) - (dx >> 7) + (t >> 2) + (t >> 6);
}

int32 FindDistance3D(int32 dx, int32 dy, int32 dz)
{
	int t;

	dx = abs(dx);
	dy = abs(dy);
	dz = abs(dz);
	if (dx < dy)
		SWAP(dx, dy);
	if (dy < dz)
		SWAP(dy, dz);

	t = dy + dz;

	return dx - (dx >> 4) + (t >> 2) + (t >> 3);
}

int32 FindDistance3D_HP(int32 dx, int32 dy, int32 dz)
{
	int x, y, z, r;
	dx >>= 5;
	dy >>= 5;
	dz >>= 5;

	x = FixedMul(dx, dx);
	y = FixedMul(dy, dy);
	z = FixedMul(dz, dz);

	r = FixedSqrtHP(x + y + z);
	r <<= 5;
	return r;
}

int32 ArcTangentAppx(int32 dx, int32 dy)
{
	int ax, ay, d, r;
	if (!dx && !dy)
		return 0;

	ax = abs(ax);
	ay = abs(ay);

	if (ax >= ay)
		d = FixedDiv2(ay, ax);
	else
		d = FixedDiv2(ax, ay);

	if (dx >= 0)
	{
		if (dy >= 0)
		{
			if (ax >= ay)
				r = d;
			else
				r = 0x20000 - d;
		}
		else
		{
			if (ax >= ay)
				r = 0x80000 - d;
			else
				r = 0x60000 + d;
		}
	}
	else
	{
		if (dy >= 0)
		{
			if (ax >= ay)
				r = 0x40000 - d;
			else
				r = 0x20000 + d;
		}
		else
		{
			if (ax >= ay)
				r = 0x40000 + d;
			else
				r = 0x60000 - d;
		}
	}

	return FixedMul(r, 256) & 2047;
}
