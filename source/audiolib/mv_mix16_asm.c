#include "compat.h"
extern char* MV_HarshClipTable;
extern char* MV_MixDestination;
extern uint32_t MV_MixPosition;
extern short* MV_LeftVolume;
extern short* MV_RightVolume;
extern int MV_SampleSize;
extern int MV_RightChannelOffset;

void MV_Mix8BitMono16(uint32_t position, uint32_t rate,
	char* start, uint32_t length)
{
	start++;

	length >>= 1;
	if (!length)
		return;

	uint32_t pos1 = position >> 16; position += rate;
	uint32_t pos2 = position >> 16; position += rate;

	int s1 = 128 + (signed char)start[pos1 * 2];
	int s2 = 128 + (signed char)start[pos2 * 2];

	char* dest = MV_MixDestination;


	do
	{
		int o1 = (signed char)MV_LeftVolume[s1] + dest[0];
		int o2 = (signed char)MV_LeftVolume[s2] + dest[MV_SampleSize];
		
		dest[0] = MV_HarshClipTable[128 + o1];
		dest[MV_SampleSize] = MV_HarshClipTable[128 + o2];

		dest += MV_SampleSize * 2;

		pos1 = position >> 16; position += rate;
		pos2 = position >> 16; position += rate;

		s1 = 128 + (signed char)start[pos1 * 2];
		s2 = 128 + (signed char)start[pos2 * 2];
	} while (--length);

	MV_MixDestination = dest;
	MV_MixPosition = position;
}

void MV_Mix8BitStereo16(uint32_t position, uint32_t rate,
	char* start, uint32_t length)
{
	start++;
	if (!length)
		return;

	uint32_t pos = position >> 16;

	int s1 = 128 + (signed char)start[pos * 2];

	char* dest = MV_MixDestination;


	do
	{
		int o1 = (signed char)MV_LeftVolume[s1] + dest[0];
		int o2 = (signed char)MV_RightVolume[s1] + dest[MV_RightChannelOffset];
		
		dest[0] = MV_HarshClipTable[128 + o1];
		dest[MV_RightChannelOffset] = MV_HarshClipTable[128 + o2];

		dest += MV_SampleSize;

		position += rate;

		pos = position >> 16;
		s1 = 128 + (signed char)start[pos * 2];
	} while (--length);

	MV_MixDestination = dest;
	MV_MixPosition = position;
}


void MV_Mix16BitMono16(uint32_t position,
	uint32_t rate, char* start, uint32_t length)
{
	short *start2 = (short*)start;
	if (!length)
		return;

	uint32_t pos1 = position >> 16; position += rate;

	int s1 = start2[pos1] ^ 32768;

	char* dest = MV_MixDestination;


	do
	{
		int o1 = (MV_LeftVolume[(char)s1] >> 8) + (short)MV_LeftVolume[(char)(s1 >> 8)] + 128 + *(short*)dest;

		if (o1 < -32768)
			o1 = -32768;
		else if (o1 > 32767)
			o1 = 32767;
		*(short*)dest = o1;

		dest += MV_SampleSize;

		pos1 = position >> 16; position += rate;

		s1 = start2[pos1] ^ 32768;
	} while (--length);

	MV_MixDestination = dest;
	MV_MixPosition = position;
}


void MV_Mix16BitStereo16(uint32_t position,
	uint32_t rate, char* start, uint32_t length)
{
	short *start2 = (short*)start;
	if (!length)
		return;

	uint32_t pos1 = position >> 16;

	int s1 = start2[pos1] ^ 32768;

	char* dest = MV_MixDestination;


	do
	{
		int o1 = (MV_LeftVolume[(char)s1] >> 8) + (short)MV_LeftVolume[(char)(s1 >> 8)] + 128 + *(short*)dest;
		int o2 = (MV_RightVolume[(char)s1] >> 8) + (short)MV_RightVolume[(char)(s1 >> 8)] + 128 + *(short*)(dest + MV_RightChannelOffset);

		if (o1 < -32768)
			o1 = -32768;
		else if (o1 > 32767)
			o1 = 32767;
		*(short*)dest = o1;

		if (o2 < -32768)
			o2 = -32768;
		else if (o2 > 32767)
			o2 = 32767;
		*(short*)(dest + MV_RightChannelOffset) = o2;

		dest += MV_SampleSize;

		position += rate;

		pos1 = position >> 16;

		s1 = start2[pos1] ^ 32768;
	} while (--length);

	MV_MixDestination = dest;
	MV_MixPosition = position;
}
