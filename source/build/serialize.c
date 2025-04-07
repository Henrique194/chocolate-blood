#include <string.h>
#include <stdlib.h>
#include "compat.h"
#include "serialize.h"
#include "pragmas.h"

static char *ser_buffer;
static int ser_buffer_size;
static int ser_ptr;
static int ser_size;

#define SER_INITIAL_BUFFER_SIZE 65536

char* Ser_Init(int size)
{
	if (size <= 0)
		return NULL;
	if (ser_buffer_size < size)
	{
		while (ser_buffer_size < size)
			ser_buffer_size += SER_INITIAL_BUFFER_SIZE;

		free(ser_buffer);
		ser_buffer = malloc(size);
	}
	ser_ptr = 0;
	ser_size = size;
	return ser_buffer;
}

void Ser_Put(void* data, int size, int is_raw)
{
	if (!ser_buffer || ser_ptr + size > ser_size)
		return;
	char* b_ptr = (char*)data;
	if (is_raw || 1)
	{
		memcpy(ser_buffer + ser_ptr, b_ptr, size);
	}
	else
	{
		// big-endian
		copybufreverse(ser_buffer + ser_ptr, b_ptr + size - 1, size);
	}
	ser_ptr += size;
}

void Ser_Get(void* data, int size, int is_raw)
{
	if (!ser_buffer || ser_ptr + size > ser_size)
		return;
	char* b_ptr = (char*)data;
	if (is_raw || 1)
	{
		memcpy(b_ptr, ser_buffer + ser_ptr, size);
	}
	else
	{
		// big-endian
		copybufreverse(b_ptr, ser_buffer + ser_ptr + size - 1, size);
	}
	ser_ptr += size;
}

void Ser_Wall(ser_func_t func, walltype* wal, int count)
{
	int i;
	for (i = 0; i < count; i++)
	{
		walltype* w = &wal[i];
		SEM(w->x);
		SEM(w->y);
		SEM(w->point2);
		SEM(w->nextwall);
		SEM(w->nextsector);
		SEM(w->cstat);
		SEM(w->picnum);
		SEM(w->overpicnum);
		SEM(w->shade);
		SEM(w->pal);
		SEM(w->xrepeat);
		SEM(w->yrepeat);
		SEM(w->xpanning);
		SEM(w->ypanning);
		SEM(w->lotag);
		SEM(w->hitag);
		SEM(w->extra);
	}
}

void Ser_Sprite(ser_func_t func, spritetype* spr, int count)
{
	int i;
	for (i = 0; i < count; i++)
	{
		spritetype* s = &spr[i];
		SEM(s->x);
		SEM(s->y);
		SEM(s->z);
		SEM(s->cstat);
		SEM(s->picnum);
		SEM(s->shade);
		SEM(s->pal);
		SEM(s->clipdist);
		SEM(s->filler);
		SEM(s->xrepeat);
		SEM(s->yrepeat);
		SEM(s->xoffset);
		SEM(s->yoffset);
		SEM(s->sectnum);
		SEM(s->statnum);
		SEM(s->ang);
		SEM(s->owner);
		SEM(s->xvel);
		SEM(s->yvel);
		SEM(s->zvel);
		SEM(s->lotag);
		SEM(s->hitag);
		SEM(s->extra);
	}
}

void Ser_Sector(ser_func_t func, sectortype* sec, int count)
{
	int i;
	for (i = 0; i < count; i++)
	{
		sectortype* s = &sec[i];
		SEM(s->wallptr);
		SEM(s->wallnum);
		SEM(s->ceilingz);
		SEM(s->floorz);
		SEM(s->ceilingstat);
		SEM(s->floorstat);
		SEM(s->ceilingpicnum);
		SEM(s->ceilingheinum);
		SEM(s->ceilingshade);
		SEM(s->ceilingpal);
		SEM(s->ceilingxpanning);
		SEM(s->ceilingypanning);
		SEM(s->floorpicnum);
		SEM(s->floorheinum);
		SEM(s->floorshade);
		SEM(s->floorpal);
		SEM(s->floorxpanning);
		SEM(s->floorypanning);
		SEM(s->visibility);
		SEM(s->filler);
		SEM(s->lotag);
		SEM(s->hitag);
		SEM(s->extra);
	}
}

int32_t Ser_GetInt32()
{
	if (!ser_buffer || ser_ptr + 4 > ser_size)
		return 0;
	int32_t ret;
	memcpy(&ret, ser_buffer + ser_ptr, 4);
	ser_ptr += 4;

	return ret;
}
