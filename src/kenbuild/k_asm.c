#include <stdlib.h>
#include "compat.h"

extern int32_t kdmasm1;
extern intptr_t kdmasm2;
extern int32_t kdmasm3;
extern intptr_t kdmasm4;

extern char qualookup[];

int32_t monolocomb(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	int64_t inc = ((int64_t)edx << 32) >> 12;
	int64_t phase = ((int64_t)esi << 32) >> 12;

	do
	{
		char d = *(char*)(kdmasm4 + (phase >> 32));
		*(int32_t*)edi += *(int32_t*)(ebx + d * 4);
		edi += 4;
		phase += inc;

		if (--ecx == 0)
			break;

		if ((uint64_t)phase < (uint64_t)inc)
		{
			if (!kdmasm1)
				break;
			kdmasm4 = kdmasm2;

			phase -= ((int64_t)kdmasm3 << 32) >> 12;
		}

	} while (1);

	return phase >> (32 - 12);
}

int32_t monohicomb(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	int64_t inc = ((int64_t)edx << 32) >> 12;
	int64_t phase = ((int64_t)esi << 32) >> 12;

	do
	{
		char l = *(char*)(kdmasm4 + (phase >> 32));
		char h = *(char*)(kdmasm4 + (phase >> 32) + 1);
		char s = l + qualookup[((phase >> 19) & 0x1e00) + ((l - h) & 0x1ff)];
		*(int32_t*)edi += *(int32_t*)(ebx + s * 4);
		edi += 4;
		phase += inc;

		if (--ecx == 0)
			break;

		if ((uint64_t)phase < (uint64_t)inc)
		{
			if (!kdmasm1)
				break;
			kdmasm4 = kdmasm2;

			phase -= ((int64_t)kdmasm3 << 32) >> 12;
		}

	} while (1);

	return phase >> (32 - 12);
}

int32_t stereolocomb(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	int64_t inc = ((int64_t)edx << 32) >> 12;
	int64_t phase = ((int64_t)esi << 32) >> 12;

	do
	{
		char d = *(char*)(kdmasm4 + (phase >> 32));
		*(int32_t*)edi += *(int32_t*)(ebx + d * 8);
		edi += 4;
		*(int32_t*)edi += *(int32_t*)(ebx + d * 8 + 4);
		edi += 4;
		phase += inc;

		if (--ecx == 0)
			break;

		if ((uint64_t)phase < (uint64_t)inc)
		{
			if (!kdmasm1)
				break;
			kdmasm4 = kdmasm2;

			phase -= ((int64_t)kdmasm3 << 32) >> 12;
		}

	} while (1);

	return phase >> (32 - 12);
}

int32_t stereohicomb(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	int64_t inc = ((int64_t)edx << 32) >> 12;
	int64_t phase = ((int64_t)esi << 32) >> 12;

	do
	{
		char l = *(char*)(kdmasm4 + (phase >> 32));
		char h = *(char*)(kdmasm4 + (phase >> 32) + 1);
		char s = l + qualookup[((phase >> 19) & 0x1e00) + ((l - h) & 0x1ff)];
		*(int32_t*)edi += *(int32_t*)(ebx + s * 8);
		edi += 4;
		*(int32_t*)edi += *(int32_t*)(ebx + s * 8 + 4);
		edi += 4;
		phase += inc;

		if (--ecx == 0)
			break;

		if ((uint64_t)phase < (uint64_t)inc)
		{
			if (!kdmasm1)
				break;
			kdmasm4 = kdmasm2;

			phase -= ((int64_t)kdmasm3 << 32) >> 12;
		}

	} while (1);

	return phase >> (32 - 12);
}


int32_t setuppctimerhandler(intptr_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, int32_t edi)
{
	return 0;
}

void pcbound2char(int32_t ecx, intptr_t esi, intptr_t edi)
{
}

void bound2char(int32_t ecx, intptr_t esi, intptr_t edi)
{
	ecx <<= 1;
	intptr_t base = edi + ecx;
	edi = -ecx;

	do
	{
		int32_t a = *(int32_t*)esi;
		int32_t b = *(int32_t*)(esi + 4);
		if (a & 0xffff0000)
		{
			if (a < 0)
				a = 0x0000;
			else
				a = 0xff00;
		}
		if (b & 0xffff0000)
		{
			if (b < 0)
				b = 0x0000;
			else
				b = 0xff00;
		}
		*(int32_t*)esi = 32768;
		*(int32_t*)(esi + 4) = 32768;

		esi += 8;

		*(char*)(base + edi) = a >> 8;
		*(char*)(base + edi + 1) = b >> 8;

		edi += 2;
	} while ((uintptr_t)edi >= (uintptr_t)2);
}

void bound2short(int32_t ecx, intptr_t esi, intptr_t edi)
{
	ecx <<= 2;
	intptr_t base = edi + ecx;
	edi = -ecx;

	do
	{
		int32_t a = *(int32_t*)esi;
		int32_t b = *(int32_t*)(esi + 4);
		if (a & 0xffff0000)
		{
			if (a < 0)
				a = 0;
			else
				a = ~0;
		}
		if (b & 0xffff0000)
		{
			if (b < 0)
				b = 0;
			else
				b = ~0;
		}
		*(int32_t*)esi = 32768;
		*(int32_t*)(esi + 4) = 32768;

		esi += 8;

		*(short*)(base + edi) = a ^ 0x8000;
		*(short*)(base + edi + 2) = b ^ 0x8000;

		edi += 4;
	} while ((uintptr_t)edi >= (uintptr_t)4);
}
