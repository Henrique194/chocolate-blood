#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include "compat.h"
#include "sound.h"
#include "kdmeng.h"
#include "cache1d.h"

#define NUMCHANNELS 16
#define MAXWAVES 256
#define MAXTRACKS 256
#define MAXNOTES 8192
#define MAXEFFECTS 16

	//Actual sound parameters after initsb was called
int32_t samplerate, numspeakers, bytespersample, intspersec, kdmqual;

	//KWV wave variables
int32_t numwaves;
char instname[MAXWAVES][16];
int32_t wavleng[MAXWAVES];
int32_t repstart[MAXWAVES], repleng[MAXWAVES];
int32_t finetune[MAXWAVES];

	//Other useful wave variables
int32_t totsndbytes, totsndmem;
intptr_t wavoffs[MAXWAVES];

	//Effects array
int32_t eff[MAXEFFECTS][256];

	//KDM song variables:
int32_t kdmversionum, numnotes, numtracks;
char trinst[MAXTRACKS], trquant[MAXTRACKS];
char trvol1[MAXTRACKS], trvol2[MAXTRACKS];
int32_t nttime[MAXNOTES];
char nttrack[MAXNOTES], ntfreq[MAXNOTES];
char ntvol1[MAXNOTES], ntvol2[MAXNOTES];
char ntfrqeff[MAXNOTES], ntvoleff[MAXNOTES], ntpaneff[MAXNOTES];

	//Other useful song variables:
int32_t timecount, notecnt, musicstatus, musicrepeat;

int32_t kdmasm1, kdmasm3;
intptr_t kdmasm2, kdmasm4;

static char digistat = 0, musistat = 0;

char *snd = NULL, kwvname[20] = {""};

#define MAXBYTESPERTIC 1024+128
static int32_t stemp[MAXBYTESPERTIC];

	//Sound reading information
int32_t splc[NUMCHANNELS], sinc[NUMCHANNELS];
intptr_t soff[NUMCHANNELS];
int32_t svol1[NUMCHANNELS], svol2[NUMCHANNELS];
int32_t volookup[NUMCHANNELS<<9];
int32_t swavenum[NUMCHANNELS];
int32_t frqeff[NUMCHANNELS], frqoff[NUMCHANNELS];
int32_t voleff[NUMCHANNELS], voloff[NUMCHANNELS];
int32_t paneff[NUMCHANNELS], panoff[NUMCHANNELS];

static int32_t globposx, globposy, globxvect, globyvect;
static intptr_t xplc[NUMCHANNELS], yplc[NUMCHANNELS];
static int32_t vol[NUMCHANNELS];
static int32_t vdist[NUMCHANNELS], sincoffs[NUMCHANNELS];
static char chanstat[NUMCHANNELS];

int32_t frqtable[256];

static int32_t mixerval = 0;

static int32_t kdminprep = 0, kdmintinprep = 0;
static int32_t dmacheckport, dmachecksiz;

//void (__interrupt __far *oldsbhandler)();
void sbhandler(void);

int32_t samplediv, oldtimerfreq, chainbackcnt, chainbackstart;
char *pcsndptr, pcsndlookup[256], bufferside;
int32_t samplecount, pcsndbufsiz, pcrealmodeint;
static short kdmvect = 0x8;
static unsigned short kdmpsel, kdmrseg, kdmroff;
static uint32_t kdmpoff;

//void (__interrupt __far *oldpctimerhandler)();
#define KDMCODEBYTES 256
static char pcrealbuffer[KDMCODEBYTES] =        //See pckdm.asm
{
	0x50,0x53,0x51,0x52,0x32,0xC0,0xE6,0x42,0xB0,0x20,
	0xE6,0x20,0x5A,0x59,0x5B,0x58,0xCF,
};

	//Sound interrupt information
int32_t sbport = 0x220, sbirq = 0x7, sbdma8 = 0x1, sbdma16 = 0x1;
char dmapagenum[8] = {0x87,0x83,0x81,0x82,0x8f,0x8b,0x89,0x8a};
int32_t sbdma, sbver;
unsigned short sndselector;
volatile int32_t sndplc, sndend;
volatile intptr_t sndoffs, sndoffsplc, sndoffsxor;
static int32_t bytespertic, sndbufsiz;

char qualookup[512*16];
int32_t ramplookup[64];

unsigned short sndseg = 0;

void getsbset();
void preparesndbuf();
void loadwaves(char *wavename);

extern int32_t monolocomb(int32_t,intptr_t,int32_t,int32_t,int32_t,intptr_t);
extern int32_t monohicomb(int32_t,intptr_t,int32_t,int32_t,int32_t,intptr_t);
extern int32_t stereolocomb(int32_t,intptr_t,int32_t,int32_t,int32_t,intptr_t);
extern int32_t stereohicomb(int32_t,intptr_t,int32_t,int32_t,int32_t,intptr_t);
extern void setuppctimerhandler(intptr_t,int32_t,int32_t,int32_t,int32_t,int32_t);
extern void pcbound2char(int32_t,intptr_t,intptr_t);
extern void bound2char(int32_t,intptr_t,intptr_t);
extern void bound2short(int32_t,intptr_t,intptr_t);

//static int32_t oneshr10 = 0x3a800000, oneshl14 = 0x46800000;

#define _USE_MATH_DEFINES
#include <math.h>

static inline void fsin(int32_t *a)
{
	double f = sin(M_PI * (double)*a * (1/1024.0)) * 16384.0;
	*a = (int32_t)f;
}

static inline void calcvolookupmono(int32_t* ptr, int32_t a, int32_t b)
{
	for (int i = 0; i < 256; i++)
	{
		ptr[i] = a;
		a += b;
	}
}

static inline void calcvolookupstereo(int32_t* ptr, int32_t a, int32_t b, int32_t c, int32_t d)
{
	for (int i = 0; i < 512; i += 2)
	{
		ptr[i+0] = a;
		ptr[i+1] = c;
		a += b;
		c += d;
	}
}

static inline int32_t klabs(int32_t a)
{
	if (a < 0)
		return -a;
	return a;
}

static inline int32_t mulscale16(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 16);
}

static inline int32_t mulscale24(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 24);
}

static inline int32_t mulscale30(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 30);
}

static inline int32_t dmulscale28(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 28);
}

static inline void clearbuf(char *ptr, uint32_t cnt, uint32_t val)
{
	while (cnt--)
	{
		*(uint32_t*)ptr = val;
		ptr += 4;
	}
}

static inline void copybuf(char* src, char* dst, uint32_t cnt)
{
	while (cnt--)
	{
		*(uint32_t*)dst = *(uint32_t*)src;
		dst += 4;
		src += 4;
	}
}

static uint32_t msqrtasm(uint32_t ecx)
{
	uint32_t eax = 0x40000000;
	uint32_t ebx = 0x20000000;
	do
	{
		if ((int32_t)ecx >= (int32_t)eax)
		{
			ecx -= 0x40000000;
			eax += ebx * 4;
		}
		eax -= ebx;
		eax >>= 1;
		ebx >>= 2;
	} while (ebx != 0);
	if ((int32_t)ecx >= (int32_t)eax)
		eax++;
	eax >>= 1;
	return eax;
}

void initsb(char dadigistat, char damusistat, int32_t dasamplerate, char danumspeakers, char dabytespersample, char daintspersec, char daquality)
{
	Sound_Init(48000);

	int32_t i, j, k;

	digistat = dadigistat;
	musistat = damusistat;

	if ((digistat == 0) && (musistat != 1))
		return;

	samplerate = dasamplerate;
	if (samplerate < 6000) samplerate = 6000;
	if (samplerate > 48000) samplerate = 48000;
	numspeakers = danumspeakers;
	if (numspeakers < 1) numspeakers = 1;
	if (numspeakers > 2) numspeakers = 2;
	bytespersample = dabytespersample;
	if (bytespersample < 1) bytespersample = 1;
	if (bytespersample > 2) bytespersample = 2;
	intspersec = daintspersec;
	if (intspersec < 1) intspersec = 1;
	if (intspersec > 120) intspersec = 120;
	kdmqual = daquality;
	if (kdmqual < 0) kdmqual = 0;
	if (kdmqual > 1) kdmqual = 1;

	switch(digistat)
	{
		case 1:
			getsbset();
			if (blaster_type == BLASTER_TYPE_NONE) { digistat = musistat = 0; break; }

			switch (blaster_type)
			{
				case BLASTER_TYPE_1:
					sbver = 0x200;
					break;
				case BLASTER_TYPE_2:
					sbver = 0x201;
					break;
				case BLASTER_TYPE_PRO:
					sbver = 0x300;
					break;
				case BLASTER_TYPE_16:
					sbver = 0x400;
					break;
			}

			if (sbver < 0x0201) samplerate = min(samplerate,22050);
			if (sbver < 0x0300) numspeakers = 1;
			if (sbver < 0x0400)
			{
				samplerate = min(samplerate,44100>>(numspeakers-1));
				bytespersample = 1;
			}

			if (bytespersample == 2) sbdma = sbdma16; else sbdma = sbdma8;

			break;
		case 2:
			//findpas();        // If == -1 then print not found & quit
			//koutp(0xf8a,128);
			break;
		case 13:
			if (numspeakers == 2) numspeakers = 1;
			if (bytespersample == 2) bytespersample = 1;
			break;
	}

	bytespertic = (((samplerate/120)+1)&~1);
	sndbufsiz = bytespertic*(120/intspersec);

	if (sndseg == 0)    //Allocate DMA buffer in conventional memory
	{
		int size = ((sndbufsiz<<(bytespersample+numspeakers-1))+15)>>4;
		if ((sndoffs = Blaster_SetDmaPageSize(size << 4)) == 0)
		{
			sys_printf("Could not allocation conventional memory for digitized music\n");
			exit(0);
		}

		//if ((sndoffs&65535)+(sndbufsiz<<(bytespersample+numspeakers-1)) >= 65536)   //64K DMA page check
		//	sndoffs += (sndbufsiz<<(bytespersample+numspeakers-1));

		sndoffsplc = sndoffs;
		sndoffsxor = sndoffsplc ^ (sndoffsplc+(sndbufsiz<<(bytespersample+numspeakers-2)));
	}
	
	j = (((11025L*2093)/samplerate)<<13);
	for(i=1;i<76;i++)
	{
		frqtable[i] = j;
		j = mulscale30(j,1137589835);  //(pow(2,1/12)<<30) = 1137589835
	}
	for(i=0;i>=-14;i--) frqtable[i&255] = (frqtable[(i+12)&255]>>1);

	loadwaves("WAVES.KWV");

	timecount = notecnt = musicstatus = musicrepeat = 0;

	clearbuf(FP_OFF(stemp),sizeof(stemp)>>2,32768L);
	for(i=0;i<256;i++)
		for(j=0;j<16;j++)
		{
			qualookup[(j<<9)+i] = (((-i)*j+8)>>4);
			qualookup[(j<<9)+i+256] = (((256-i)*j+8)>>4);
		}
	for(i=0;i<(samplerate>>11);i++)
	{
		j = 1536 - (i<<10)/(samplerate>>11);
		fsin(&j);
		ramplookup[i] = ((16384-j)<<1);
	}

	for(i=0;i<256;i++)
	{
		j = i*90; fsin(&j);
		eff[0][i] = 65536+j/9;
		eff[1][i] = min(58386+((i*(65536-58386))/30),65536);
		eff[2][i] = max(69433+((i*(65536-69433))/30),65536);
		j = (i*2048)/120; fsin(&j);
		eff[3][i] = 65536+(j<<2);
		j = (i*2048)/30; fsin(&j);
		eff[4][i] = 65536+j;
		switch((i>>1)%3)
		{
			case 0: eff[5][i] = 65536; break;
			case 1: eff[5][i] = 65536*330/262; break;
			case 2: eff[5][i] = 65536*392/262; break;
		}
		eff[6][i] = min((i<<16)/120,65536);
		eff[7][i] = max(65536-(i<<16)/120,0);
	}

	switch(digistat)
	{
		case 1: case 2:
			Blaster_SetIrqHandler(sbhandler);
#if 0
			if (sbirq < 8)
			{
				oldsbhandler = _dos_getvect(sbirq+0x8);           //Set new IRQ7 vector
				_disable(); _dos_setvect(sbirq+0x8, sbhandler); _enable();
				koutp(0x21,kinp(0x21) & ~(1<<(sbirq&7)));
			}
			else
			{
				oldsbhandler = _dos_getvect(sbirq+0x68);    //Set new SB IRQ vector
				_disable(); _dos_setvect(sbirq+0x68, sbhandler); _enable();
				koutp(0xa1,kinp(0xa1) & ~(1<<(sbirq&7)));
			}
#endif
			break;
	}

	musicoff();

	if (digistat != 255)
	{
		preparesndbuf();
		if (digistat != 13) preparesndbuf();

		if ((digistat == 1) || (digistat == 2))
		{
			if (sbver < 0x0400)
				dmachecksiz = (sndbufsiz<<(bytespersample+numspeakers-1))-1;
			else
				dmachecksiz = ((sndbufsiz<<(bytespersample+numspeakers-1))>>1)-1;
#if 0
			if (sbdma < 4)
			{
				dmacheckport = (sbdma<<1)+1;
				dmachecksiz = (sndbufsiz<<(bytespersample+numspeakers-1))-1;

				koutp(0xa,sbdma+4);             //Set up DMA REGISTERS
				koutp(0xc,0);
				koutp(0xb,0x58+sbdma);          //&H58 - auto-init, &H48 - 1 block only
				koutp(sbdma<<1,sndoffs&255);
				koutp(sbdma<<1,(sndoffs>>8)&255);
				koutp(dmacheckport,dmachecksiz&255);
				koutp(dmacheckport,(dmachecksiz>>8)&255);
				koutp(dmapagenum[sbdma],((sndoffs>>16)&255));
				koutp(0xa,sbdma);
			}
			else
			{
				dmacheckport = ((sbdma&3)<<2)+0xc2;
				dmachecksiz = ((sndbufsiz<<(bytespersample+numspeakers-1))>>1)-1;

				koutp(0xd4,sbdma);               //Set up DMA REGISTERS
				koutp(0xd8,0);
				koutp(0xd6,0x58+(sbdma&3));      //&H58 - auto-init, &H48 - 1 block only
				koutp(dmacheckport-2,(sndoffs>>1)&255);
				koutp(dmacheckport-2,((sndoffs>>1)>>8)&255);
				koutp(dmacheckport,dmachecksiz&255);
				koutp(dmacheckport,(dmachecksiz>>8)&255);
				koutp(dmapagenum[sbdma],((sndoffs>>16)&255));
				koutp(0xd4,sbdma&3);
			}
#endif
		}

		switch(digistat)
		{
			case 1:
			{
				int tc = -1;
				if (sbver < 0x0400)
				{
					tc = 256-(1000000/(samplerate<<(numspeakers-1)));
				}

				if (sbver < 0x0200)
				{
					Blaster_Init(tc, -1, 1, 0);
					Blaster_StartDma(0, dmachecksiz, sndbufsiz-1, 0);
				}
				else
				{
					if (sbver < 0x0400)
					{
						Blaster_Init(tc, -1, numspeakers, 0);
							//Set length for auto-init mode
						i = (sndbufsiz<<(numspeakers+bytespersample-2))-1;
						Blaster_StartDma(0, dmachecksiz, i, 1);
					}
					else
					{
						Blaster_Init(-1, samplerate, numspeakers, bytespersample - 1);
						i = (sndbufsiz<<(numspeakers-1))-1;
						Blaster_StartDma(0, dmachecksiz, i, 1);
					}
				}
				break;
			}
			case 2:
#if 0
				koutp(0xf88,128);
				koutp(0xb8a,0);

				i = (1193180L>>(numspeakers-1)) / samplerate;
				koutp(0x138b,0x36); koutp(0x1388,i&255); koutp(0x1388,i>>8);
				i = (sndbufsiz<<(bytespersample+numspeakers-2));
				koutp(0x138b,0x74); koutp(0x1389,i&255); koutp(0x1389,i>>8);

				koutp(0x8389,0x3+((bytespersample-1)<<2)); //0x3=8bit/0x7=16bit
				koutp(0xb89,0xdf);
				koutp(0xb8b,0x8);
				koutp(0xf8a,0xd9+((2-numspeakers)<<5));    //0xd9=play/0xc9=record
				koutp(0xb8a,0xe1);
#endif
				break;
			case 13:
#if 0
				samplecount = sndbufsiz;
				pcsndptr = (char *)sndoffs;
				bufferside = 0;
				pcsndbufsiz = sndbufsiz;
				pcrealmodeint = 0;

				samplediv = 1193280L / samplerate;

				j = 0;
				for(i=0;i<256;i++)
				{
						 //Scale (65536 normal)
					k = mulscale24(j-(samplediv<<7),160000) + (samplediv>>1);
					if (k < 0) k = 0;
					if (k > samplerate) k = samplerate;
					pcsndlookup[i] = (char)(k+1);
					j += samplediv;
				}

				oldtimerfreq = gettimerval();
				chainbackstart = oldtimerfreq/samplediv;
				chainbackcnt = chainbackstart;
				setuppctimerhandler(sndoffs+sndbufsiz,oldtimerfreq,0L,0L,0L,0L);

				//_disable();
				//oldpctimerhandler = _dos_getvect(0x8);
				//installbikdmhandlers();
				//koutp(0x43,0x34); koutp(0x40,samplediv&255); koutp(0x40,(samplediv>>8)&255);
				//koutp(0x43,0x90);
				//koutp(97,kinp(97)|3);
				//_enable();
#endif
				break;
		}
	}
}

void getsbset()
{
	sbport = 0x220;
	sbirq = 5;
	sbdma8 = 1;
	sbdma16 = 3;
#if 0
	char *sbset;
	int32_t i;

	sbset = getenv("BLASTER");

	i = 0;
	while (sbset[i] > 0)
	{
		switch(sbset[i])
		{
			case 'A': case 'a':
				i++;
				sbport = 0;
				while (((sbset[i] >= 48) && (sbset[i] <= 57)) ||
						 ((sbset[i] >= 'A') && (sbset[i] <= 'F')) ||
						 ((sbset[i] >= 'a') && (sbset[i] <= 'f')))
				{
					sbport *= 16;
					if ((sbset[i] >= 48) && (sbset[i] <= 57)) sbport += sbset[i]-48;
					if ((sbset[i] >= 'A') && (sbset[i] <= 'F')) sbport += sbset[i]-55;
					if ((sbset[i] >= 'a') && (sbset[i] <= 'f')) sbport += sbset[i]-55-32;
					i++;
				}
				break;
			case 'I': case 'i':
				i++;
				sbirq = 0;
				while ((sbset[i] >= 48) && (sbset[i] <= 57))
				{
					sbirq *= 10;
					sbirq += sbset[i]-48;
					i++;
				}
				break;
			case 'D': case 'd':
				i++;
				sbdma8 = 0;
				while ((sbset[i] >= 48) && (sbset[i] <= 57))
				{
					sbdma8 *= 10;
					sbdma8 += sbset[i]-48;
					i++;
				}
				break;
			case 'H': case 'h':
				i++;
				sbdma16 = 0;
				while ((sbset[i] >= 48) && (sbset[i] <= 57))
				{
					sbdma16 *= 10;
					sbdma16 += sbset[i]-48;
					i++;
				}
				break;
			default:
				i++;
				break;
		}
	}
#endif
}

void sbhandler()
{
	switch(digistat)
	{
		case 1:
#if 0
			if (sbver < 0x0200)
			{
				sbout(0x14);                           //SB 1-shot mode
				sbout((sndbufsiz-1)&255);
				sbout(((sndbufsiz-1)>>8)&255);
				kinp(sbport+0xe);                      //Acknowledge SB
			}
			else
			{
				mixerval = sbmixin(0x82);
				if (mixerval&1) kinp(sbport+0xe);      //Acknowledge 8-bit DMA
				if (mixerval&2) kinp(sbport+0xf);      //Acknowledge 16-bit DMA
			}
#endif
			break;
		case 2:
			//if ((kinp(0xb89)&8) > 0) koutp(0xb89,0);
			break;
	}
	/*if (sbirq >= 8) koutp(0xa0, 0x20);
	koutp(0x20,0x20);
	_enable(); */preparesndbuf();
}

void uninitsb()
{
	if ((digistat == 0) && (musistat != 1))
		return;

	if (digistat != 255)
	{
		//if ((digistat == 1) || (digistat == 2))  //Mask off DMA
		//{
		//	if (sbdma < 4) koutp(0xa,sbdma+4); else koutp(0xd4,sbdma);
		//}

		switch(digistat)
		{
			case 1:
				//if (sbver >= 0x0400) sbout(0xda-(bytespersample-1));
				//resetsb();
				//sbout(0xd3);                           //Turn speaker off
				break;
			case 2:
				//koutp(0xb8a,32);                       //Stop interrupts
				//koutp(0xf8a,0x9);                      //DMA stop
				break;
			case 13:
				//koutp(97,kinp(97)&252);
				//koutp(0x43,0x34); koutp(0x40,0); koutp(0x40,0);
				//koutp(0x43,0xbc);
				//uninstallbikdmhandlers();
				break;
		}
	}

	if (snd != 0) free(snd), snd = 0;
	//if (sndseg != 0) convdeallocate(sndseg), sndseg = 0;

	switch(digistat)
	{
		case 1: case 2:
			//if (sbirq < 8)
			//{
			//	koutp(0x21,kinp(0x21) | (1<<(sbirq&7)));
			//	_disable(); _dos_setvect(sbirq+0x8, oldsbhandler); _enable();
			//}
			//else
			//{
			//	koutp(0xa1,kinp(0xa1) | (1<<(sbirq&7)));
			//	_disable(); _dos_setvect(sbirq+0x68, oldsbhandler); _enable();
			//}
			Blaster_SetIrqHandler(NULL);
			break;
	}
}

void startwave(int32_t wavnum, int32_t dafreq, int32_t davolume1, int32_t davolume2, int32_t dafrqeff, int32_t davoleff, int32_t dapaneff)
{
	int32_t i, j, chanum;

	if ((davolume1|davolume2) == 0) return;

	chanum = 0;
	for(i=NUMCHANNELS-1;i>0;i--)
		if (splc[i] > splc[chanum])
			chanum = i;

	splc[chanum] = 0;     //Disable channel temporarily for clean switch

	if (numspeakers == 1)
		calcvolookupmono(FP_OFF(volookup)+(chanum<<(9+2)),-(davolume1+davolume2)<<6,(davolume1+davolume2)>>1);
	else
		calcvolookupstereo(FP_OFF(volookup)+(chanum<<(9+2)),-(davolume1<<7),davolume1,-(davolume2<<7),davolume2);

	sinc[chanum] = dafreq;
	svol1[chanum] = davolume1;
	svol2[chanum] = davolume2;
	soff[chanum] = wavoffs[wavnum]+wavleng[wavnum];
	splc[chanum] = -(wavleng[wavnum]<<12);              //splc's modified last
	swavenum[chanum] = wavnum;
	frqeff[chanum] = dafrqeff; frqoff[chanum] = 0;
	voleff[chanum] = davoleff; voloff[chanum] = 0;
	paneff[chanum] = dapaneff; panoff[chanum] = 0;
	chanstat[chanum] = 0; sincoffs[chanum] = 0;
}

void setears(int32_t daposx, int32_t daposy, int32_t daxvect, int32_t dayvect)
{
	globposx = daposx;
	globposy = daposy;
	globxvect = daxvect;
	globyvect = dayvect;
}

void wsayfollow(char *dafilename, int32_t dafreq, int32_t davol, int32_t *daxplc, int32_t *dayplc, char followstat)
{
	char ch1, ch2, bad;
	int32_t i, wavnum, chanum;

	if (digistat == 0) return;
	if (davol <= 0) return;

	for(wavnum=numwaves-1;wavnum>=0;wavnum--)
	{
		bad = 0;

		i = 0;
		while ((dafilename[i] > 0) && (i < 16))
		{
			ch1 = dafilename[i]; if ((ch1 >= 97) && (ch1 <= 123)) ch1 -= 32;
			ch2 = instname[wavnum][i]; if ((ch2 >= 97) && (ch2 <= 123)) ch2 -= 32;
			if (ch1 != ch2) {bad = 1; break;}
			i++;
		}
		if (bad != 0) continue;

		chanum = 0;
		for(i=NUMCHANNELS-1;i>0;i--) if (splc[i] > splc[chanum]) chanum = i;

		splc[chanum] = 0;     //Disable channel temporarily for clean switch

		if (followstat == 0)
		{
			xplc[chanum] = *daxplc;
			yplc[chanum] = *dayplc;
		}
		else
		{
			xplc[chanum] = ((intptr_t)daxplc);
			yplc[chanum] = ((intptr_t)dayplc);
		}
		vol[chanum] = davol;
		vdist[chanum] = 0;
		sinc[chanum] = (dafreq*11025)/samplerate;
		svol1[chanum] = davol;
		svol2[chanum] = davol;
		sincoffs[chanum] = 0;
		soff[chanum] = wavoffs[wavnum]+wavleng[wavnum];
		splc[chanum] = -(wavleng[wavnum]<<12);              //splc's modified last
		swavenum[chanum] = wavnum;
		chanstat[chanum] = followstat+1;
		frqeff[chanum] = 0; frqoff[chanum] = 0;
		voleff[chanum] = 0; voloff[chanum] = 0;
		paneff[chanum] = 0; panoff[chanum] = 0;
		return;
	}
}

void getsndbufinfo(int32_t *dasndoffsplc, int32_t *dasndbufsiz)
{
	*dasndoffsplc = sndoffsplc;
	*dasndbufsiz = (sndbufsiz<<(bytespersample+numspeakers-2));
}

void preparesndbuf()
{
	int32_t i, j, k, voloffs1, voloffs2, *stempptr;
	int32_t daswave, dasinc, dacnt;
	int32_t ox, oy, x, y;
	intptr_t voloffs1_p, voloffs2_p;
	char *sndptr, v1, v2;

	kdmintinprep++;
	if (kdminprep != 0) return;

	if ((digistat == 1) || (digistat == 2))
	{
#if 0
		i = kinp(dmacheckport); i += (kinp(dmacheckport)<<8);
#endif
		i = Blaster_GetDmaCount();
		if (i <= dmachecksiz)
		{
			i = ((i > 32) && (i <= (dmachecksiz>>1)+32));
			if ((sndoffsplc<(sndoffsplc^sndoffsxor)) == i) kdmintinprep++;
		}
	}

	kdminprep = 1;
	while (kdmintinprep > 0)
	{
		sndoffsplc ^= sndoffsxor;

		for (i=NUMCHANNELS-1;i>=0;i--)
			if ((splc[i] < 0) && (chanstat[i] > 0))
			{
				if (chanstat[i] == 1)
				{
					ox = xplc[i];
					oy = yplc[i];
				}
				else
				{
					stempptr = (int32_t *)xplc[i]; ox = *stempptr;
					stempptr = (int32_t *)yplc[i]; oy = *stempptr;
				}
				ox -= globposx; oy -= globposy;
				x = dmulscale28(oy,globxvect,-ox,globyvect);
				y = dmulscale28(ox,globxvect,oy,globyvect);

				if ((klabs(x) >= 32768) || (klabs(y) >= 32768))
					{ splc[i] = 0; continue; }

				j = vdist[i];
				vdist[i] = msqrtasm(x*x+y*y);
				if (j)
				{
					j = (sinc[i]<<10)/(min(max(vdist[i]-j,-768),768)+1024)-sinc[i];
					sincoffs[i] = ((sincoffs[i]*7+j)>>3);
				}

				voloffs1 = min((vol[i]<<22)/(((x+1536)*(x+1536)+y*y)+1),255);
				voloffs2 = min((vol[i]<<22)/(((x-1536)*(x-1536)+y*y)+1),255);

				if (numspeakers == 1)
					calcvolookupmono(FP_OFF(volookup)+(i<<(9+2)),-(voloffs1+voloffs2)<<6,(voloffs1+voloffs2)>>1);
				else
					calcvolookupstereo(FP_OFF(volookup)+(i<<(9+2)),-(voloffs1<<7),voloffs1,-(voloffs2<<7),voloffs2);
			}

		for(dacnt=0;dacnt<sndbufsiz;dacnt+=bytespertic)
		{
			if (musicstatus > 0)    //Gets here 120 times/second
			{
				while ((notecnt < numnotes) && (timecount >= nttime[notecnt]))
				{
					j = trinst[nttrack[notecnt]];
					k = mulscale24(frqtable[ntfreq[notecnt]],finetune[j]+748);

					if (ntvol1[notecnt] == 0)   //Note off
					{
						for(i=NUMCHANNELS-1;i>=0;i--)
							if (splc[i] < 0)
								if (swavenum[i] == j)
									if (sinc[i] == k)
										splc[i] = 0;
					}
					else                        //Note on
						startwave(j,k,ntvol1[notecnt],ntvol2[notecnt],ntfrqeff[notecnt],ntvoleff[notecnt],ntpaneff[notecnt]);

					notecnt++;
					if (notecnt >= numnotes)
						if (musicrepeat > 0)
							notecnt = 0, timecount = nttime[0];
				}
				timecount++;
			}

			for(i=NUMCHANNELS-1;i>=0;i--)
			{
				if (splc[i] >= 0) continue;

				dasinc = sinc[i]+sincoffs[i];

				if (frqeff[i] != 0)
				{
					dasinc = mulscale16(dasinc,eff[frqeff[i]-1][frqoff[i]]);
					frqoff[i]++; if (frqoff[i] >= 256) frqeff[i] = 0;
				}
				if ((voleff[i]) || (paneff[i]))
				{
					voloffs1 = svol1[i];
					voloffs2 = svol2[i];
					if (voleff[i])
					{
						voloffs1 = mulscale16(voloffs1,eff[voleff[i]-1][voloff[i]]);
						voloffs2 = mulscale16(voloffs2,eff[voleff[i]-1][voloff[i]]);
						voloff[i]++; if (voloff[i] >= 256) voleff[i] = 0;
					}

					if (numspeakers == 1)
						calcvolookupmono(FP_OFF(volookup)+(i<<(9+2)),-(voloffs1+voloffs2)<<6,(voloffs1+voloffs2)>>1);
					else
					{
						if (paneff[i])
						{
							voloffs1 = mulscale16(voloffs1,131072-eff[paneff[i]-1][panoff[i]]);
							voloffs2 = mulscale16(voloffs2,eff[paneff[i]-1][panoff[i]]);
							panoff[i]++; if (panoff[i] >= 256) paneff[i] = 0;
						}
						calcvolookupstereo(FP_OFF(volookup)+(i<<(9+2)),-(voloffs1<<7),voloffs1,-(voloffs2<<7),voloffs2);
					}
				}

				daswave = swavenum[i];
				voloffs1_p = FP_OFF(volookup)+(i<<(9+2));

				kdmasm1 = repleng[daswave];
				kdmasm2 = wavoffs[daswave]+repstart[daswave]+repleng[daswave];
				kdmasm3 = (repleng[daswave]<<12); //repsplcoff
				kdmasm4 = soff[i];
				if (numspeakers == 1)
				{
					if (kdmqual == 0) splc[i] = monolocomb(0L,voloffs1_p,bytespertic,dasinc,splc[i],FP_OFF(stemp));
									 else splc[i] = monohicomb(0L,voloffs1_p,bytespertic,dasinc,splc[i],FP_OFF(stemp));
				}
				else
				{
					if (kdmqual == 0) splc[i] = stereolocomb(0L,voloffs1_p,bytespertic,dasinc,splc[i],FP_OFF(stemp));
									 else splc[i] = stereohicomb(0L,voloffs1_p,bytespertic,dasinc,splc[i],FP_OFF(stemp));
				}
				soff[i] = kdmasm4;

				if ((kdmqual == 0) || (splc[i] >= 0)) continue;
				if (numspeakers == 1)
				{
					if (kdmqual == 0) monolocomb(0L,voloffs1_p,samplerate>>11,dasinc,splc[i],FP_OFF(stemp)+(bytespertic<<2));
									 else monohicomb(0L,voloffs1_p,samplerate>>11,dasinc,splc[i],FP_OFF(stemp)+(bytespertic<<2));
				}
				else
				{
					if (kdmqual == 0) stereolocomb(0L,voloffs1_p,samplerate>>11,dasinc,splc[i],FP_OFF(stemp)+(bytespertic<<3));
									 else stereohicomb(0L,voloffs1_p,samplerate>>11,dasinc,splc[i],FP_OFF(stemp)+(bytespertic<<3));
				}
			}

			if (kdmqual)
			{
				if (numspeakers == 1)
				{
					for(i=(samplerate>>11)-1;i>=0;i--)
						stemp[i] += mulscale16(stemp[i+1024]-stemp[i],ramplookup[i]);
					j = bytespertic; k = (samplerate>>11);
					copybuf(&stemp[j],&stemp[1024],k);
					clearbuf(&stemp[j],k,32768);
				}
				else
				{
					for(i=(samplerate>>11)-1;i>=0;i--)
					{
						j = (i<<1);
						stemp[j+0] += mulscale16(stemp[j+1024]-stemp[j+0],ramplookup[i]);
						stemp[j+1] += mulscale16(stemp[j+1025]-stemp[j+1],ramplookup[i]);
					}
					j = (bytespertic<<1); k = ((samplerate>>11)<<1);
					copybuf(&stemp[j],&stemp[1024],k);
					clearbuf(&stemp[j],k,32768);
				}
			}

			if (numspeakers == 1)
			{
				if (bytespersample == 1)
				{
					if (digistat == 13) pcbound2char(bytespertic>>1,FP_OFF(stemp),sndoffsplc+dacnt);
										  else bound2char(bytespertic>>1,FP_OFF(stemp),sndoffsplc+dacnt);
				} else bound2short(bytespertic>>1,FP_OFF(stemp),sndoffsplc+(dacnt<<1));
			}
			else
			{
				if (bytespersample == 1) bound2char(bytespertic,FP_OFF(stemp),sndoffsplc+(dacnt<<1));
										 else bound2short(bytespertic,FP_OFF(stemp),sndoffsplc+(dacnt<<2));
			}
		}
		kdmintinprep--;
	}
	kdminprep = 0;
}

void wsay(char *dafilename, int32_t dafreq, int32_t volume1, int32_t volume2)
{
	unsigned char ch1, ch2;
	int32_t i, j, bad;

	if (digistat == 0) return;

	i = numwaves-1;
	do
	{
		bad = 0;

		j = 0;
		while ((dafilename[j] > 0) && (j < 16))
		{
			ch1 = dafilename[j]; if ((ch1 >= 97) && (ch1 <= 123)) ch1 -= 32;
			ch2 = instname[i][j]; if ((ch2 >= 97) && (ch2 <= 123)) ch2 -= 32;
			if (ch1 != ch2) {bad = 1; break;}
			j++;
		}
		if (bad == 0)
		{
			startwave(i,(dafreq*11025)/samplerate,volume1,volume2,0L,0L,0L);
			return;
		}

		i--;
	} while (i >= 0);
}

void loadwaves(char *wavename)
{
	int32_t fil, i, j, dawaversionum;
	char filename[80];

	strcpy(filename,wavename);
	if (strstr(filename,".KWV") == 0) strcat(filename,".KWV");
	if ((fil = kopen4load(filename,0)) == -1)
		if (strcmp(filename,"WAVES.KWV") != 0)
		{
			strcpy(filename,"WAVES.KWV");
			fil = kopen4load(filename,0);
		}

	totsndbytes = 0;

	if (fil != -1)
	{
		if (strcmp(kwvname,filename) == 0) { kclose(fil); return; }
		strcpy(kwvname,filename);

		kread(fil,&dawaversionum,4);
		if (dawaversionum != 0) { kclose(fil); return; }

		kread(fil,&numwaves,4);
		for(i=0;i<numwaves;i++)
		{
			kread(fil,&instname[i][0],16);
			kread(fil,&wavleng[i],4);
			kread(fil,&repstart[i],4);
			kread(fil,&repleng[i],4);
			kread(fil,&finetune[i],4);
			wavoffs[i] = totsndbytes;
			totsndbytes += wavleng[i];
		}
	}
	else
	{
		dawaversionum = 0;
		numwaves = 0;
	}

	for(i=numwaves;i<MAXWAVES;i++)
	{
		for(j=0;j<16;j++) instname[i][j] = 0;
		wavoffs[i] = totsndbytes;
		wavleng[i] = 0L;
		repstart[i] = 0L;
		repleng[i] = 0L;
		finetune[i] = 0L;
	}

	if (snd == 0)
	{
		if ((snd = (char *)malloc(totsndbytes+2)) == 0)
			{ sys_printf("Not enough memory for digital music!\n"); exit(0); }
	}
	for(i=0;i<MAXWAVES;i++) wavoffs[i] += FP_OFF(snd);
	if (fil != -1)
	{
		kread(fil,snd,totsndbytes);
		kclose(fil);
	}
	snd[totsndbytes] = snd[totsndbytes+1] = 128;
}

int loadsong(char *filename)
{
	int32_t i, fil;

	if (musistat != 1) return(0);
	musicoff();
	char* f2 = strdup(filename);
	f2 = strupr(f2);
	if (strstr(f2,".KDM") == 0) strcat(f2,".KDM");
	if ((fil = kopen4load(f2,0)) == -1)
	{
		sys_printf("I cannot load %s.\n", f2);
		uninitsb();
		free(f2);
		return(-1);
	}
	free(f2);
	kread(fil,&kdmversionum,4);
	if (kdmversionum != 0) return(-2);
	kread(fil,&numnotes,4);
	kread(fil,&numtracks,4);
	kread(fil,trinst,numtracks);
	kread(fil,trquant,numtracks);
	kread(fil,trvol1,numtracks);
	kread(fil,trvol2,numtracks);
	kread(fil,nttime,numnotes<<2);
	kread(fil,nttrack,numnotes);
	kread(fil,ntfreq,numnotes);
	kread(fil,ntvol1,numnotes);
	kread(fil,ntvol2,numnotes);
	kread(fil,ntfrqeff,numnotes);
	kread(fil,ntvoleff,numnotes);
	kread(fil,ntpaneff,numnotes);
	kclose(fil);
	return(0);
}

void musicon()
{
	if (musistat != 1)
		return;

	notecnt = 0;
	timecount = nttime[notecnt];
	musicrepeat = 1;
	musicstatus = 1;
}

void musicoff()
{
	int32_t i;

	musicstatus = 0;
	for(i=0;i<NUMCHANNELS;i++)
		splc[i] = 0;
	musicrepeat = 0;
	timecount = 0;
	notecnt = 0;
}

#if 0
kdmconvalloc32 (int32_t size)
{
	union REGS r;

	r.x.eax = 0x0100;           //DPMI allocate DOS memory
	r.x.ebx = ((size+15)>>4);   //Number of paragraphs requested
	int386(0x31,&r,&r);

	if (r.x.cflag != 0)         //Failed
		return ((int32_t)0);
	return ((int32_t)((r.x.eax&0xffff)<<4));   //Returns full 32-bit offset
}

installbikdmhandlers()
{
	union REGS r;
	struct SREGS sr;
	int32_t lowp;
	void far *fh;

		//Get old protected mode handler
	r.x.eax = 0x3500+kdmvect;   /* DOS get vector (INT 0Ch) */
	sr.ds = sr.es = 0;
	int386x(0x21,&r,&r,&sr);
	kdmpsel = (unsigned short)sr.es;
	kdmpoff = r.x.ebx;

		//Get old real mode handler
	r.x.eax = 0x0200;   /* DPMI get real mode vector */
	r.h.bl = kdmvect;
	int386(0x31,&r,&r);
	kdmrseg = (unsigned short)r.x.ecx;
	kdmroff = (unsigned short)r.x.edx;


		//Allocate memory in low memory to store real mode handler
	if ((lowp = kdmconvalloc32(KDMCODEBYTES)) == 0)
	{
		sys_printf("Can't allocate conventional memory.\n");
		exit;
	}
	memcpy((void *)lowp,(void *)pcrealbuffer,KDMCODEBYTES);

		//Set new protected mode handler
	r.x.eax = 0x2500+kdmvect;   /* DOS set vector (INT 0Ch) */
	fh = (void far *)pctimerhandler;
	r.x.edx = FP_OFF(fh);
	sr.ds = FP_SEG(fh);      //DS:EDX == &handler
	sr.es = 0;
	int386x(0x21,&r,&r,&sr);

		//Set new real mode handler (must be after setting protected mode)
	r.x.eax = 0x0201;
	r.h.bl = kdmvect;              //CX:DX == real mode &handler
	r.x.ecx = ((lowp>>4)&0xffff);  //D32realseg
	r.x.edx = 0L;                  //D32realoff
	int386(0x31,&r,&r);
}

uninstallbikdmhandlers()
{
	union REGS r;
	struct SREGS sr;

		//restore old protected mode handler
	r.x.eax = 0x2500+kdmvect;   /* DOS set vector (INT 0Ch) */
	r.x.edx = kdmpoff;
	sr.ds = kdmpsel;    /* DS:EDX == &handler */
	sr.es = 0;
	int386x(0x21,&r,&r,&sr);

		//restore old real mode handler
	r.x.eax = 0x0201;   /* DPMI set real mode vector */
	r.h.bl = kdmvect;
	r.x.ecx = (uint32_t)kdmrseg;     //CX:DX == real mode &handler
	r.x.edx = (uint32_t)kdmroff;
	int386(0x31,&r,&r);
}
#endif
