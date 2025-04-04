#include "compat.h"


void MV_16BitReverb(char* src, char* dest, short* volume, int count)
{
	do
	{
		short s = *(short*)src;

		*(short*)dest = (volume[(char)s] >> 8) + volume[(char)(s >> 8) ^ 128] + 128;

		src += 2;
		dest += 2;
	} while (--count);
}

void MV_8BitReverb(signed char* src, signed char* dest, short* volume, int count)
{
	do
	{
		char s = *(char*)src;

		*dest = (char)volume[s] + 128;

		src++;
		dest++;
	} while (--count);
}

void MV_16BitReverbFast(char* src, char* dest, int count, int shift)
{
	do
	{
		short s = *(short*)src;

		*(short*)dest = s >> shift;

		src += 2;
		dest += 2;
	} while (--count);
}

void MV_8BitReverbFast(signed char* src, signed char* dest, int count, int shift)
{
	int d = 128 - (128 >> shift);
	do
	{
		char s = *(char*)src;

		*dest = d + (s >> shift) + ((s ^ 128) >> 7);

		src++;
		dest++;
	} while (--count);
}
