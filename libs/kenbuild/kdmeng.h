#pragma once
#include "compat.h"

void initsb(char dadigistat, char damusistat, int32_t dasamplerate, char danumspeakers, char dabytespersample, char daintspersec, char daquality);
int loadsong(char *filename);
void musicon();
void musicoff();
void uninitsb();
void preparesndbuf();
void wsayfollow(char *dafilename, int32_t dafreq, int32_t davol, int32_t *daxplc, int32_t *dayplc, char followstat);
void setears(int32_t daposx, int32_t daposy, int32_t daxvect, int32_t dayvect);
void wsay(char *dafilename, int32_t dafreq, int32_t volume1, int32_t volume2);


