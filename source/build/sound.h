#pragma once

typedef enum
{
	BLASTER_TYPE_NONE = 0,
	BLASTER_TYPE_1,
	BLASTER_TYPE_2,
	BLASTER_TYPE_PRO,
	BLASTER_TYPE_16,
};

extern int blaster_type;
void Sound_Init(int rate);
void Blaster_SetIrqHandler(void (*handler)());
char *Blaster_SetDmaPageSize(int size);
void Blaster_Init(int tc, int rate, int channels, int is16bit);
void Blaster_StartDma(int start, int dma_count, int count, int is_auto);
int Blaster_GetDmaCount();
void Adlib_Write(int offset, int data);
