#pragma once
#include "build.h"

typedef void (*ser_func_t)(void* data, int size, int raw);

char *Ser_Init(int size);
void Ser_Put(void* data, int size, int raw);
void Ser_Get(void* data, int size, int raw);
int32_t Ser_GetInt32();

void Ser_Wall(ser_func_t func, walltype* wal, int count);
void Ser_Sprite(ser_func_t func, spritetype* spr, int count);
void Ser_Sector(ser_func_t func, sectortype* sec, int count);

#define SEI(x) sb = Ser_Init(x)
#define SEM(x) func(&(x), sizeof(x), 0)
#define SEMR(x) func(&(x), sizeof(x), 1)
