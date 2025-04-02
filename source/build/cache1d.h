#pragma once
#include <stdio.h>
#include "compat.h"


void initcache(intptr_t dacachestart, int32_t dacachesize);
void allocache(intptr_t *newhandle, int32_t newbytes, char *newlockptr);
void suckcache(int32_t *suckptr);
void agecache();


int32_t initgroupfile(char *filename);
void uninitgroupfile();
int32_t kopen4load(char *filename, char searchfirst);
int32_t kread(int32_t handle, void *buffer, int32_t leng);
int32_t klseek(int32_t handle, int32_t offset, int32_t whence);
int32_t kfilelength(int32_t handle);
void kclose(int32_t handle);

void kdfread(void *buffer, size_t dasizeof, size_t count, int32_t fil);
void dfread(void *buffer, size_t dasizeof, size_t count, FILE *fil);
void dfwrite(void *buffer, size_t dasizeof, size_t count, FILE *fil);
