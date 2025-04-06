// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.
// This file has been modified from Ken Silverman's original release
#include "compat.h"

extern int32_t asm1, asm4;
extern intptr_t asm2, asm3;
extern int32_t reciptable[];
extern union {
	int32_t i;
	float f;
} fpuasm;

typedef union {
	uint16_t lw;
	struct {
		uint32_t l, h;
	};
	uint64_t m;
} texcoord64_t;

typedef union {
	struct {
		uint8_t a, b, c, d;
	};
	uint32_t m;
} col32_t;

typedef union {
	struct {
		uint8_t a, b;
	};
	uint16_t m;
} col16_t;

#if 0

typedef struct {
	texcoord64_t l;
	uint32_t h;
} texcoord96_t;
#endif

extern int32_t globalx3, globaly3;
extern int32_t ylookup[];
extern int32_t vplce[];
extern int32_t vince[];
extern intptr_t palookupoffse[];
extern intptr_t bufplce[];
extern char pow2char[];
extern int32_t pow2long[];

static uint8_t sethlinesizes_al, sethlinesizes_bl;
static uint8_t sethlinesizes_al_n, sethlinesizes_bl_n;
static intptr_t sethlinesizes_ecx;
static uint32_t sethlinesizes_mask;
void sethlinesizes(int32_t eax, int32_t ebx, intptr_t ecx)
{
	sethlinesizes_al = eax;
	sethlinesizes_al_n = 32 - eax;
	sethlinesizes_bl = ebx;
	sethlinesizes_bl_n = 32 - ebx;
	sethlinesizes_ecx = ecx;

	sethlinesizes_mask = (uint32_t)~0 >> (32 - eax - ebx);
}

static int32_t setvlinebpl_eax;
void setvlinebpl(int32_t eax)
{
	setvlinebpl_eax = eax;
}

static char* setpalookupaddress_eax;
void setpalookupaddress(char* eax)
{
	setpalookupaddress_eax = eax;
}

static uint64_t hlineasm4_sub;
void setuphlineasm4(int32_t eax, int32_t ebx)
{
	hlineasm4_sub = (uint32_t)(eax << sethlinesizes_al);
	hlineasm4_sub |= (uint32_t)eax >> (32 - sethlinesizes_al);
	uint32_t h = ebx;
	h &= ~255;
	h |= hlineasm4_sub & 255;
	hlineasm4_sub |= (uint64_t)h << 32;
}

void hlineasm4(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	int32_t ebp = eax + 1;
	if (ebp > 8)
	{
		if ((edi & 1) == 0)
		{
			uint32_t a = (uint32_t)esi >> sethlinesizes_al_n;
			a <<= sethlinesizes_bl;
			a |= (uint32_t)edx >> sethlinesizes_bl_n;
			*(char*)edi = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a) + ecx];
			esi -= asm1;
			edx -= asm2;
			edi--;
			ebp--;
		}
		if ((edi & 2) == 0)
		{
			uint32_t a = (uint32_t)esi >> sethlinesizes_al_n;
			a <<= sethlinesizes_bl;
			a |= (uint32_t)edx >> sethlinesizes_bl_n;
			char bh = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a) + ecx];
			esi -= asm1;
			edx -= asm2;
			a = (uint32_t)esi >> sethlinesizes_al_n;
			a <<= sethlinesizes_bl;
			a |= (uint32_t)edx >> sethlinesizes_bl_n;
			char bl = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a) + ecx];
			esi -= asm1;
			edx -= asm2;
			*(uint16_t*)(edi - 1) = (bh << 8) | bl;
			edi -= 2;
			ebp -= 2;
		}
		texcoord64_t crd;
		crd.l = esi;
		crd.h = edx;

		crd.h &= ~255;
		crd.h |= ((uint32_t)crd.l >> sethlinesizes_al_n) & 255;
		crd.l <<= sethlinesizes_al;

		edi++;

		if (ebx >= 0)
		{
			hlineasm4_sub = (uint32_t)(asm1 << sethlinesizes_al);
			hlineasm4_sub |= (uint32_t)asm1 >> (32 - sethlinesizes_al);
			uint32_t h = asm2;
			h &= ~255;
			h |= hlineasm4_sub & 255;
			hlineasm4_sub |= (uint64_t)h << 32;
		}
		while (1)
		{
			uint32_t a = (uint32_t)crd.h << sethlinesizes_bl;
			a |= (uint32_t)crd.h >> (32 - sethlinesizes_bl);
			a &= sethlinesizes_mask;
			crd.m -= hlineasm4_sub;

			edi -= 4;

			col32_t col;
			col.d = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a) + ecx];

			a = (uint32_t)crd.h << sethlinesizes_bl;
			a |= (uint32_t)crd.h >> (32 - sethlinesizes_bl);
			a &= sethlinesizes_mask;
			crd.m -= hlineasm4_sub;

			col.c = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a) + ecx];

			a = (uint32_t)crd.h << sethlinesizes_bl;
			a |= (uint32_t)crd.h >> (32 - sethlinesizes_bl);
			a &= sethlinesizes_mask;
			crd.m -= hlineasm4_sub;

			col.b = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a) + ecx];

			a = (uint32_t)crd.h << sethlinesizes_bl;
			a |= (uint32_t)crd.h >> (32 - sethlinesizes_bl);
			a &= sethlinesizes_mask;
			crd.m -= hlineasm4_sub;

			col.a = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a) + ecx];

			ebp -= 4;
			if ((uint32_t)ebp < (uint32_t)-4)
			{
				*(uint32_t*)edi = col.m;
				continue;
			}

			if (ebp & 2)
			{
				*(short*)(edi + 2) = col.m >> 16;
				col.d = col.b;
				edi -= 2;
			}
			if (ebp & 1)
			{
				*(char*)(edi + 3) = col.d;
			}
			break;
		}
	}
	else
	{
		while (ebp)
		{
			uint32_t a = (uint32_t)esi >> sethlinesizes_al_n;
			a <<= sethlinesizes_bl;
			a |= (uint32_t)edx >> sethlinesizes_bl_n;
			*(char*)edi = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a) + ecx];
			esi -= asm1;
			edx -= asm2;
			edi--;
			ebp--;
		}
	}
}

static uint32_t setupvlineasm_al;
static uint32_t setupvlineasm_al_n16;
static uint32_t setupvlineasm_al_n;
static uint32_t setupvlineasm_al_mask;

void setupvlineasm(int32_t eax)
{
	setupvlineasm_al = eax;
	setupvlineasm_al_n = 32 - eax;
	setupvlineasm_al_n16 = 16 - eax;
	setupvlineasm_al_mask = (1 << setupvlineasm_al_n) - 1;
}
static uint32_t setupmvlineasm_al;

void setupmvlineasm(int32_t eax)
{
	setupmvlineasm_al = eax;
}

static uint32_t setuptvlineasm_al;
void setuptvlineasm(int32_t eax)
{
	setuptvlineasm_al = eax;
}

int32_t vlineasm1(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, intptr_t esi, intptr_t edi);

int32_t prevlineasm1(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, intptr_t esi, intptr_t edi)
{
	if (ecx)
		return vlineasm1(eax, ebx, ecx, edx, esi, edi);

	eax += edx;
	uint32_t a = (uint32_t)edx >> setupvlineasm_al;
	*(char*)edi = *(char*)(ebx + *(char*)(esi + a));
	return eax;
}

int32_t vlineasm1(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, intptr_t esi, intptr_t edi)
{
	ecx++;
	edi -= setvlinebpl_eax;
	do
	{
		uint32_t a = (uint32_t)edx >> setupvlineasm_al;
		edx += eax;
		edi += setvlinebpl_eax;
		*(char*)edi = *(char*)(ebx + *(char*)(esi + a));
	} while (--ecx);

	return edx;
}

int32_t mvlineasm1(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, intptr_t esi, intptr_t edi)
{
	do
	{
		uint32_t a = (uint32_t)edx >> setupmvlineasm_al;
		edx += eax;
		char c = *(char*)(esi + a);
		if (c != 255)
			*(char*)edi = *(char*)(ebx + c);
		edi += setvlinebpl_eax;
	} while (--ecx >= 0);

	return edx;
}

static intptr_t fixtransluscence_eax;
void fixtransluscence(intptr_t eax)
{
	fixtransluscence_eax = eax;
}

static char trans_s1 = 0;
static char trans_s2 = 8;

void settransnormal()
{
	trans_s1 = 0;
	trans_s2 = 8;
}

void settransreverse()
{
	trans_s1 = 8;
	trans_s2 = 0;
}

int32_t tvlineasm1(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, intptr_t esi, intptr_t edi)
{
	ecx++;
	do
	{
		uint32_t a = (uint32_t)edx >> setuptvlineasm_al;
		edx += eax;
		char c = *(char*)(esi + a);
		if (c != 255)
		{
			c = *(char*)(ebx + c);
			*(char*)edi = *(char*)(fixtransluscence_eax + (*(char*)edi << trans_s2) + (c << trans_s1));
		}
		edi += setvlinebpl_eax;
	} while (--ecx);

	return edx;
}

void vlineasm4(int32_t ecx, intptr_t edi)
{
	intptr_t v1 = edi + ylookup[ecx];
	edi -= v1;

	uint32_t va = ((uint32_t)vince[3] << setupvlineasm_al_n) | (((uint32_t)vince[3] >> (32 - setupvlineasm_al_n)));
	uint32_t vb = ((uint32_t)vince[1] << setupvlineasm_al_n) | (((uint32_t)vince[1] >> (32 - setupvlineasm_al_n)));
	uint64_t add1 = va & 0xffff0000;
	add1 |= (uint64_t)((vince[2] & 0xfffffe00) + (va & 0x1ff)) << 32;
	uint16_t add2_l = vb >> 16;
	uint32_t add2_h = (vince[0] & 0xfffffe00) + (vb & 0x1ff);

	va = ((uint32_t)vplce[3] << setupvlineasm_al_n) | (((uint32_t)vplce[3] >> (32 - setupvlineasm_al_n)));
	vb = ((uint32_t)vplce[1] << setupvlineasm_al_n) | (((uint32_t)vplce[1] >> (32 - setupvlineasm_al_n)));
	texcoord64_t crd;
	crd.l = (va & 0xffff0000) | (vb >> 16);
	crd.h = (vplce[2] & 0xfffffe00) + (va & 0x1ff);
	uint32_t crd_h2 = (vplce[0] & 0xfffffe00) + (vb & 0x1ff);

	do
	{
		uint32_t o0 = crd_h2 >> setupvlineasm_al;
		uint32_t o1 = crd_h2 & setupvlineasm_al_mask;
		uint32_t o2 = crd.h >> setupvlineasm_al;
		uint32_t o3 = crd.h & setupvlineasm_al_mask;

		crd.m += add1;

		crd.lw += add2_l;
		if (crd.lw < add2_l)
			crd_h2++;
		crd_h2 += add2_h;

		*(char*)&o0 = *(char*)(bufplce[0] + o0);
		*(char*)&o1 = *(char*)(bufplce[1] + o1);
		*(char*)&o2 = *(char*)(bufplce[2] + o2);
		*(char*)&o3 = *(char*)(bufplce[3] + o3);

		col32_t col;
		col.a = *(char*)(palookupoffse[0] + o0);
		col.b = *(char*)(palookupoffse[1] + o1);
		col.c = *(char*)(palookupoffse[2] + o2);
		col.d = *(char*)(palookupoffse[3] + o3);

		*(uint32_t*)(v1 + edi) = col.m;

		edi += setvlinebpl_eax;
	} while (edi < 0);

	vplce[2] = crd.h;
	vplce[0] = crd_h2;
	vplce[3] = (crd.h << setupvlineasm_al) + (crd.l >> setupvlineasm_al_n);
	vplce[1] = (crd_h2 << setupvlineasm_al) + ((crd.l >> setupvlineasm_al_n16) & 0xffff);
}

void mvlineasm4(int32_t ecx, intptr_t edi)
{
	uint32_t plc0 = vplce[0];
	uint32_t plc1 = vplce[1];
	uint32_t plc2 = vplce[2];
	uint32_t plc3 = vplce[3];

	char* cl_ptr = (char*)&plc0;
	char* asm3_ptr = (char*)&asm3;

	*cl_ptr = ecx + 1;
	*asm3_ptr = (ecx >> 8) + 1;

	edi -= setvlinebpl_eax;
	uint32_t cas = 0;
	do
	{
		while (--*cl_ptr)
		{
			uint32_t o0 = plc0 >> setupmvlineasm_al;
			uint32_t o1 = plc1 >> setupmvlineasm_al;
			uint32_t o2 = plc2 >> setupmvlineasm_al;
			uint32_t o3 = plc3 >> setupmvlineasm_al;

			*(char*)&o0 = *(char*)(bufplce[0] + o0);
			*(char*)&o1 = *(char*)(bufplce[1] + o1);
			*(char*)&o2 = *(char*)(bufplce[2] + o2);
			*(char*)&o3 = *(char*)(bufplce[3] + o3);

			plc0 += vince[0] & ~255;
			plc1 += vince[1] & ~255;
			plc2 += vince[2];
			plc3 += vince[3];

			cas <<= 1;
			cas |= (char)o3 < 255;
			cas <<= 1;
			cas |= (char)o2 < 255;
			cas <<= 1;
			cas |= (char)o1 < 255;
			cas <<= 1;
			cas |= (char)o0 < 255;

			col32_t col;
			col.a = *(char*)(palookupoffse[0] + o0);
			col.b = *(char*)(palookupoffse[1] + o1);
			col.c = *(char*)(palookupoffse[2] + o2);
			col.d = *(char*)(palookupoffse[3] + o3);

			edi += setvlinebpl_eax;

			switch (cas & 15)
			{
				case 0:
					break;
				case 1:
					*(char*)(edi) = col.a;
					break;
				case 2:
					*(char*)(edi + 1) = col.b;
					break;
				case 3:
					*(short*)(edi) = (short)col.m;
					break;
				case 4:
					*(char*)(edi + 2) = col.c;
					break;
				case 5:
					*(char*)(edi) = col.a;
					*(char*)(edi + 2) = col.c;
					break;
				case 6:
					*(short*)(edi + 1) = (short)(col.m >> 8);
					break;
				case 7:
					*(short*)(edi) = (short)col.m;
					*(char*)(edi + 2) = col.c;
					break;
				case 8:
					*(char*)(edi + 3) = col.d;
					break;
				case 9:
					*(char*)(edi) = col.a;
					*(char*)(edi + 3) = col.d;
					break;
				case 10:
					*(char*)(edi + 1) = col.b;
					*(char*)(edi + 3) = col.c;
					break;
				case 11:
					*(short*)(edi) = (short)col.m;
					*(char*)(edi + 3) = col.d;
					break;
				case 12:
					*(short*)(edi + 2) = (short)(col.m >> 16);
					break;
				case 13:
					*(char*)(edi) = col.a;
					*(short*)(edi + 2) = (short)(col.m >> 16);
					break;
				case 14:
					*(char*)(edi + 1) = col.b;
					*(short*)(edi + 2) = (short)(col.m >> 16);
					break;
				case 15:
					*(uint32_t*)edi = col.m;
					break;
			}
		}
	} while (--*asm3_ptr);
	vplce[0] = plc0;
	vplce[1] = plc1;
	vplce[2] = plc2;
	vplce[3] = plc3;
}

static intptr_t setupspritevline_eax;
static int32_t setupspritevline_esi;
static int32_t setupspritevline_ebx;
static int32_t setupspritevline_ecx;
static int32_t setupspritevline_edx;
void setupspritevline(intptr_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, int32_t edi)
{
	setupspritevline_eax = eax;
	setupspritevline_esi = esi << 16;
	setupspritevline_ebx = ebx + (esi >> 16);
	setupspritevline_edx = setupspritevline_ebx + edx;
	setupspritevline_ecx = ecx;
}

void spritevline(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, intptr_t esi, intptr_t edi)
{
	while (--ecx)
	{
		char c = *(char*)esi;
		*(char*)edi = *(char*)(setupspritevline_eax + c);
		edi += setvlinebpl_eax;

		edx += setupspritevline_ecx;
		if ((uint32_t)edx >= (uint32_t)setupspritevline_ecx)
			esi += setupspritevline_ebx;
		else
			esi += setupspritevline_edx;
		ebx += setupspritevline_esi;
		if ((uint32_t)ebx < (uint32_t)setupspritevline_esi)
			esi++;
	}
}

static intptr_t msetupspritevline_eax;
static int32_t msetupspritevline_esi;
static int32_t msetupspritevline_ebx;
static int32_t msetupspritevline_ecx;
static int32_t msetupspritevline_edx;
void msetupspritevline(intptr_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, int32_t edi)
{
	msetupspritevline_eax = eax;
	msetupspritevline_esi = esi << 16;
	msetupspritevline_ebx = ebx + (esi >> 16);
	msetupspritevline_edx = msetupspritevline_ebx + edx;
	msetupspritevline_ecx = ecx;
}

void mspritevline(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, intptr_t esi, intptr_t edi)
{
	while (--ecx)
	{
		char c = *(char*)esi;
		if (c != 255)
			*(char*)edi = *(char*)(setupspritevline_eax + c);
		edi += setvlinebpl_eax;

		edx += msetupspritevline_ecx;
		if ((uint32_t)edx >= (uint32_t)msetupspritevline_ecx)
			esi += msetupspritevline_ebx;
		else
			esi += msetupspritevline_edx;
		ebx += msetupspritevline_esi;
		if ((uint32_t)ebx < (uint32_t)msetupspritevline_esi)
			esi++;
	}
}

static intptr_t tsetupspritevline_eax;
static int32_t tsetupspritevline_esi;
static int32_t tsetupspritevline_ebx;
static int32_t tsetupspritevline_ecx;
static int32_t tsetupspritevline_edx;
void tsetupspritevline(intptr_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, int32_t edi)
{
	tsetupspritevline_eax = eax;
	tsetupspritevline_esi = esi << 16;
	tsetupspritevline_ebx = ebx + (esi >> 16);
	tsetupspritevline_edx = tsetupspritevline_ebx + edx;
	tsetupspritevline_ecx = ecx;
}

void tspritevline(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, intptr_t esi, intptr_t edi)
{
	while (--ecx)
	{
		char c = *(char*)esi;
		if (c != 255)
		{
			c = *(char*)(tsetupspritevline_eax + c);
			*(char*)edi = *(char*)(fixtransluscence_eax + (*(char*)edi << trans_s2) + (c << trans_s1));
		}
		edi += setvlinebpl_eax;

		edx += tsetupspritevline_ecx;
		if ((uint32_t)edx >= (uint32_t)tsetupspritevline_ecx)
			esi += tsetupspritevline_ebx;
		else
			esi += tsetupspritevline_edx;
		ebx += tsetupspritevline_esi;
		if ((uint32_t)ebx < (uint32_t)tsetupspritevline_esi)
			esi++;
	}
}

static uint8_t msethlineshift_al_n;
static uint8_t msethlineshift_bl;
void msethlineshift(int32_t eax, int32_t ebx)
{
	msethlineshift_al_n = 32 - eax;
	msethlineshift_bl = ebx;
}

void mhlineskipmodify(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi);

static intptr_t mhline_eax;
static int32_t mhline_asm1;
static int32_t mhline_asm2;
static intptr_t mhline_asm3;
void mhline(intptr_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	mhline_eax = eax;
	mhline_asm1 = asm1;
	mhline_asm2 = asm2;
	mhline_asm3 = asm3;

	mhlineskipmodify(0, ebx, ecx, edx, esi, edi);
}

void mhlineskipmodify(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	if (!(ecx & 65536))
	{
		uint32_t b = (uint32_t)ebx >> msethlineshift_al_n;
		b <<= msethlineshift_bl;
		b |= (uint32_t)esi >> (32 - msethlineshift_bl);
		
		ebx += asm1;
		esi += asm2;

		char c = *(char*)(mhline_eax + b);
		if (c != 255)
			*(char*)edi = *(char*)(mhline_asm3 + c);
		edi++;
		ecx -= 65536;
		if ((uint32_t)ecx >= (uint32_t)(-65536))
			return;
	}

	do
	{
		uint32_t b1 = (uint32_t)ebx >> msethlineshift_al_n;
		b1 <<= msethlineshift_bl;
		b1 |= (uint32_t)esi >> (32 - msethlineshift_bl);
		ebx += mhline_asm1;
		esi += mhline_asm2;
		uint32_t b2 = (uint32_t)ebx >> msethlineshift_al_n;
		b2 <<= msethlineshift_bl;
		b2 |= (uint32_t)esi >> (32 - msethlineshift_bl);
		ebx += mhline_asm1;
		esi += mhline_asm2;
		char c1 = *(char*)(mhline_eax + b1);
		char c2 = *(char*)(mhline_eax + b2);
		if (c1 != 255 && c2 != 255)
			*(short*)edi = *(char*)(mhline_asm3 + c1) | (*(char*)(mhline_asm3 + c2) << 8);
		else if (c1 != 255)
			*(char*)edi = *(char*)(mhline_asm3 + c1);
		else if (c2 != 255)
			*(char*)(edi + 1) = *(char*)(mhline_asm3 + c2);

		edi += 2;
		ecx -= 131072;
		if ((uint32_t)ecx >= (uint32_t)(-131072))
			return;
	} while (1);
}

static uint8_t tsethlineshift_al_n;
static uint8_t tsethlineshift_bl;
void tsethlineshift(int32_t eax, int32_t ebx)
{
	tsethlineshift_al_n = 32 - eax;
	tsethlineshift_bl = ebx;
}

void thlineskipmodify(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi);

static intptr_t thline_eax;
static int32_t thline_asm1;
static int32_t thline_asm2;
static intptr_t thline_asm3;
void thline(intptr_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	thline_eax = eax;
	thline_asm1 = asm1;
	thline_asm2 = asm2;
	thline_asm3 = asm3;

	thlineskipmodify(0, ebx, ecx, edx, esi, edi);
}

void thlineskipmodify(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	if (!(ecx & 65536))
	{
		uint32_t b = (uint32_t)ebx >> tsethlineshift_al_n;
		b <<= tsethlineshift_bl;
		b |= (uint32_t)esi >> (32 - tsethlineshift_bl);
		
		ebx += asm1;
		esi += asm2;

		char c = *(char*)(thline_eax + b);
		if (c != 255)
		{
			c = *(char*)(thline_asm3 + c);
			*(char*)edi = *(char*)(fixtransluscence_eax + (*(char*)edi << trans_s2) + (c << trans_s1));
		}
		edi++;
		ecx -= 65536;
		if ((uint32_t)ecx >= (uint32_t)(-65536))
			return;
	}

	do
	{
		uint32_t b1 = (uint32_t)ebx >> tsethlineshift_al_n;
		b1 <<= tsethlineshift_bl;
		b1 |= (uint32_t)esi >> (32 - tsethlineshift_bl);
		ebx += thline_asm1;
		esi += thline_asm2;
		uint32_t b2 = (uint32_t)ebx >> tsethlineshift_al_n;
		b2 <<= tsethlineshift_bl;
		b2 |= (uint32_t)esi >> (32 - tsethlineshift_bl);
		ebx += thline_asm1;
		esi += thline_asm2;
		char c1 = *(char*)(thline_eax + b1);
		char c2 = *(char*)(thline_eax + b2);
		col16_t c;
		c.m = *(short*)edi;
		if (c1 != 255)
		{
			c1 = *(char*)(thline_asm3 + c1);
			*(char*)edi = *(char*)(fixtransluscence_eax + (c.a << trans_s2) + (c1 << trans_s1));
		}
		if (c2 != 255)
		{
			c2 = *(char*)(thline_asm3 + c2);
			*(char*)(edi + 1) = *(char*)(fixtransluscence_eax + (c.b << trans_s2) + (c2 << trans_s1));
		}

		edi += 2;
		ecx -= 131072;
		if ((uint32_t)ecx >= (uint32_t)(-131072))
			return;
	} while (1);
}

static uint8_t setuptvlineasm2_al;
static intptr_t setuptvlineasm2_ebx;
static intptr_t setuptvlineasm2_ecx;
void setuptvlineasm2(int32_t eax, intptr_t ebx, intptr_t ecx)
{
	setuptvlineasm2_al = eax;
	setuptvlineasm2_ebx = ebx;
	setuptvlineasm2_ecx = ecx;
}

void tvlineasm2(int32_t eax, int32_t ebx, intptr_t ecx, intptr_t edx, int32_t esi, int32_t edi)
{
	edi -= asm2;
	uint32_t c1 = 0;
	uint32_t d2 = 0;
	do
	{
		uint32_t a = (uint32_t)esi >> setuptvlineasm2_al;
		uint32_t b = (uint32_t)eax >> setuptvlineasm2_al;
		esi += ebx;
		eax += asm1;

		char c1 = *(char*)(ecx + a);
		char c2 = *(char*)(ecx + b);

		if (c1 != 255 && c2 != 255)
		{
			col16_t c;
			c.m = *(short*) (edi + asm2);

			c1 = *(char*)(setuptvlineasm2_ebx + c1);
			c2 = *(char*)(setuptvlineasm2_ecx + c2);
			c.a = *(char*)(fixtransluscence_eax + (c.a << trans_s2) + (c1 << trans_s1));
			c.b = *(char*)(fixtransluscence_eax + (c.b << trans_s2) + (c2 << trans_s1));

			*(short*)(edi + asm2) = c.m;
		}
		else if (c1 != 255)
		{
			c1 = *(char*)(setuptvlineasm2_ebx + c1);
			*(char*)(edi + asm2) = *(char*)(fixtransluscence_eax + (*(char*)(edi + asm2) << trans_s2) + (c1 << trans_s1));
		}
		else if (c2 != 255)
		{
			c2 = *(char*)(setuptvlineasm2_ecx + c2);
			*(char*)(edi + asm2 + 1) = *(char*)(fixtransluscence_eax + (*(char*)(edi + asm2 + 1) << trans_s2) + (c2 << trans_s1));
		}

		edi += setvlinebpl_eax;

	} while ((uintptr_t)edi >= (uintptr_t)(setvlinebpl_eax));

	asm1 = esi;
	asm2 = eax;
}

static intptr_t setupslopevlin_ebx;
static int32_t setupslopevlin_ecx;
static int32_t setupslopevlin_ecx_n;
static uint32_t setupslopevlin_al_mask;
static uint8_t setupslopevlin_ah_n;
static uint8_t setupslopevlin_as;
void setupslopevlin(int32_t eax, intptr_t ebx, int32_t ecx)
{
	setupslopevlin_ebx = ebx;
	setupslopevlin_ecx = ecx;
	setupslopevlin_ecx_n = -ecx;

	uint8_t al = eax;
	uint8_t ah = eax >> 8;

	setupslopevlin_ah_n = 32 - ah;
	setupslopevlin_as = 32 - ah - al;

	setupslopevlin_al_mask = ((1 << al) - 1) << ah;

	union
	{
		float f;
		int32_t i;
	} u;

	u.f = asm1;
	asm2 = u.i;
}

#define BITSOFPRECISION 3
#define BITSOFPRECISIONPOW 8

void slopevlin(intptr_t eax, int32_t ebx, intptr_t ecx, int32_t edx, int32_t esi, int32_t edi)
{
	float f = asm3;
	f += *(float*)&asm2;

	intptr_t ebp = eax + setupslopevlin_ecx_n;

	asm1 = ebx;
	esi += (ebx << 3) * globalx3;
	edi += (ebx << 3) * globaly3;

	do
	{
		fpuasm.f = f;
		int32_t neg = fpuasm.i < 0 ? -1 : 0;
		uint8_t ex = (fpuasm.i >> 23) & 255;
		ex -= 2;
		uint32_t man = (fpuasm.i >> 12) & 2047;
		int32_t v = reciptable[man] >> ex;
		v ^= neg;

		int32_t o = asm1;
		asm1 = v;

		int32_t c = (v - o) * globalx3;
		int32_t a = (v - o) * globaly3;

		f += *(float*)&asm2;

		asm4 = edx;
		*(char*)&c = edx;

		if (edx >= BITSOFPRECISIONPOW)
			*(char*)&c = BITSOFPRECISIONPOW;



		do
		{
			uint32_t b = (uint32_t)esi >> setupslopevlin_as;
			uint32_t d = (uint32_t)edi >> setupslopevlin_ah_n;

			esi += c;
			edi += a;

			ebp += setupslopevlin_ecx;

			*(char*)&d = *(char*)(setupslopevlin_ebx + (b & setupslopevlin_al_mask) + d);

			intptr_t lookup = *(intptr_t*)(ecx);
			ecx -= sizeof(intptr_t);

			*(char*)ebp = *(char*)(lookup + d);
		} while (--*(char*)&c);


		edx = asm4 - BITSOFPRECISIONPOW;
	} while (edx > 0);
}

static int32_t setuprhlineasm4_eax;
static int32_t setuprhlineasm4_ebx;
static int32_t setuprhlineasm4_ecx;
static intptr_t setuprhlineasm4_edx;
static int32_t setuprhlineasm4_esi;
void setuprhlineasm4(int32_t eax, int32_t ebx, int32_t ecx, intptr_t edx, int32_t esi, int32_t edi)
{
	setuprhlineasm4_eax = eax;
	setuprhlineasm4_ebx = ebx;
	setuprhlineasm4_ecx = ecx;
	setuprhlineasm4_edx = edx;
	setuprhlineasm4_esi = esi;
}

void rhlineasm4(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	if (eax <= 0)
		return;

	edi = edi - 4 - eax;

	if (eax & 3)
	{
		do
		{
			char c = *(char*)ebx;

			edx -= setuprhlineasm4_eax;
			if ((uint32_t)edx >= (uint32_t)(-setuprhlineasm4_eax) && setuprhlineasm4_eax)
				ebx -= setuprhlineasm4_esi;
			esi -= setuprhlineasm4_ebx;
			if ((uint32_t)esi >= (uint32_t)(-setuprhlineasm4_ebx) && setuprhlineasm4_ebx)
				ebx--;
			ebx -= setuprhlineasm4_ecx;

			*(char*)(edi + eax + 3) = *(char*)(setuprhlineasm4_edx + c);

			eax--;
		} while (eax & 3);
		if (eax == 0)
			return;
	}

	do
	{
		col32_t col;
		col.d = *(char*)(setuprhlineasm4_edx + *(char*)ebx);

		edx -= setuprhlineasm4_eax;
		if ((uint32_t)edx >= (uint32_t)(-setuprhlineasm4_eax) && setuprhlineasm4_eax)
			ebx -= setuprhlineasm4_esi;
		esi -= setuprhlineasm4_ebx;
		if ((uint32_t)esi >= (uint32_t)(-setuprhlineasm4_ebx) && setuprhlineasm4_ebx)
			ebx--;
		ebx -= setuprhlineasm4_ecx;

		col.c = *(char*)(setuprhlineasm4_edx + *(char*)ebx);

		edx -= setuprhlineasm4_eax;
		if ((uint32_t)edx >= (uint32_t)(-setuprhlineasm4_eax) && setuprhlineasm4_eax)
			ebx -= setuprhlineasm4_esi;
		esi -= setuprhlineasm4_ebx;
		if ((uint32_t)esi >= (uint32_t)(-setuprhlineasm4_ebx) && setuprhlineasm4_ebx)
			ebx--;
		ebx -= setuprhlineasm4_ecx;

		col.b = *(char*)(setuprhlineasm4_edx + *(char*)ebx);

		edx -= setuprhlineasm4_eax;
		if ((uint32_t)edx >= (uint32_t)(-setuprhlineasm4_eax) && setuprhlineasm4_eax)
			ebx -= setuprhlineasm4_esi;
		esi -= setuprhlineasm4_ebx;
		if ((uint32_t)esi >= (uint32_t)(-setuprhlineasm4_ebx) && setuprhlineasm4_ebx)
			ebx--;
		ebx -= setuprhlineasm4_ecx;

		col.a = *(char*)(setuprhlineasm4_edx + *(char*)ebx);

		edx -= setuprhlineasm4_eax;
		if ((uint32_t)edx >= (uint32_t)(-setuprhlineasm4_eax) && setuprhlineasm4_eax)
			ebx -= setuprhlineasm4_esi;
		esi -= setuprhlineasm4_ebx;
		if ((uint32_t)esi >= (uint32_t)(-setuprhlineasm4_ebx) && setuprhlineasm4_ebx)
			ebx--;
		ebx -= setuprhlineasm4_ecx;


		*(uint32_t*)(edi + eax) = col.m;

		eax -= 4;
	} while (eax);
}


static int32_t setuprmhlineasm4_eax;
static int32_t setuprmhlineasm4_ebx;
static int32_t setuprmhlineasm4_ecx;
static intptr_t setuprmhlineasm4_edx;
static int32_t setuprmhlineasm4_esi;
void setuprmhlineasm4(int32_t eax, int32_t ebx, int32_t ecx, intptr_t edx, int32_t esi, int32_t edi)
{
	setuprmhlineasm4_eax = eax;
	setuprmhlineasm4_ebx = ebx;
	setuprmhlineasm4_ecx = ecx;
	setuprmhlineasm4_edx = edx;
	setuprmhlineasm4_esi = esi;
}

void rmhlineasm4(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	if (eax <= 0)
		return;

	edi = edi - 1 - eax;

	do
	{
		char c = *(char*)ebx;

		edx -= setuprmhlineasm4_eax;
		if ((uint32_t)edx >= (uint32_t)(-setuprmhlineasm4_eax) && setuprmhlineasm4_eax)
			ebx -= setuprmhlineasm4_esi;
		esi -= setuprmhlineasm4_ebx;
		if ((uint32_t)esi >= (uint32_t)(-setuprmhlineasm4_ebx) && setuprmhlineasm4_ebx)
			ebx--;
		ebx -= setuprmhlineasm4_ecx;

		if (c != 255)
			*(char*)(edi + eax) = *(char*)(setuprmhlineasm4_edx + c);

		eax--;
	} while (eax);
}

static int32_t setupqrhlineasm4_ebx;
static int32_t setupqrhlineasm4_ecx;
static int32_t setupqrhlineasm4_ecx_n;
static int32_t setupqrhlineasm4_ebx2;
static int32_t setupqrhlineasm4_ecx2;
static intptr_t setupqrhlineasm4_edx;
extern void setupqrhlineasm4(int32_t eax, int32_t ebx, int32_t ecx, intptr_t edx, int32_t esi, int32_t edi)
{
	setupqrhlineasm4_ebx = ebx;
	setupqrhlineasm4_ecx = ecx;
	setupqrhlineasm4_ecx_n = -ecx;
	setupqrhlineasm4_ebx2 = ebx * 2;
	setupqrhlineasm4_ecx2 = ecx * 2;
	if (ebx < 0)
		setupqrhlineasm4_ecx2++;
	setupqrhlineasm4_edx = edx;
}

void qrhlineasm4(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, int32_t esi, intptr_t edi)
{
	if (eax <= 0)
		return;

	if (eax & 3)
	{
		do
		{
			char c = *(char*)ebx;

			esi -= setupqrhlineasm4_ebx;
			if ((uint32_t)esi >= (uint32_t)(-setupqrhlineasm4_ebx) && setupqrhlineasm4_ebx)
				ebx--;
			ebx -= setupqrhlineasm4_ecx;

			edi--;
			*(char*)(edi) = *(char*)(setupqrhlineasm4_edx + c);

			eax--;
		} while (eax & 3);
		if (eax == 0)
			return;
	}

	do
	{
		col32_t col;
		col.d = *(char*)(setupqrhlineasm4_edx + *(char*)ebx);
		col.c = *(char*)(setupqrhlineasm4_edx + *(char*)ebx + setupqrhlineasm4_ecx_n);

		esi -= setupqrhlineasm4_ebx2;
		if ((uint32_t)esi >= (uint32_t)(-setupqrhlineasm4_ebx2) && setupqrhlineasm4_ebx2)
			ebx--;
		ebx -= setupqrhlineasm4_ecx2;

		col.b = *(char*)(setupqrhlineasm4_edx + *(char*)ebx);
		col.a = *(char*)(setupqrhlineasm4_edx + *(char*)ebx + setupqrhlineasm4_ecx_n);

		esi -= setupqrhlineasm4_ebx2;
		if ((uint32_t)esi >= (uint32_t)(-setupqrhlineasm4_ebx2) && setupqrhlineasm4_ebx2)
			ebx--;
		ebx -= setupqrhlineasm4_ecx2;

		edi -= 4;
		*(uint32_t*)edi = col.m;

		eax -= 4;
	} while (eax);
}


static int32_t setupdrawslab_eax;
static intptr_t setupdrawslab_ebx;
void setupdrawslab(int32_t eax, intptr_t ebx)
{
	setupdrawslab_eax = eax;
	setupdrawslab_ebx = ebx;
}

void drawslab(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, intptr_t esi, intptr_t edi)
{
	if (eax == 2)
	{
		do
		{
			char c = *(char*)(esi + ((uint32_t)ebx >> 16));
			col16_t col;
			col.a = col.b = *(char*)(setupdrawslab_ebx + c);
			*(short*)edi = col.m;
			edi += setupdrawslab_eax;
			ebx += edx;
		} while (--ecx);
	}
	else if (eax == 1)
	{
		do
		{
			char c = *(char*)(esi + ((uint32_t)ebx >> 16));
			*(char*)edi = *(char*)(setupdrawslab_ebx + c);
			edi += setupdrawslab_eax;
			ebx += edx;
		} while (--ecx);
	}
	else if (eax == 4)
	{
		do
		{
			char c = *(char*)(esi + ((uint32_t)ebx >> 16));
			col32_t col;
			col.a = col.b = col.c = col.d = *(char*)(setupdrawslab_ebx + c);
			*(uint32_t*)edi = col.m;
			edi += setupdrawslab_eax;
			ebx += edx;
		} while (--ecx);
	}
	else
	{
		intptr_t a = edi + eax;
		if ((edi & 1) && edi != a)
		{
			intptr_t edi2 = edi;
			int32_t ebx2 = ebx;
			uint32_t ecx2 = ecx;
			do
			{
				char c = *(char*)(esi + ((uint32_t)ebx2 >> 16));
				*(char*)edi2 = *(char*)(setupdrawslab_ebx + c);
				edi2 += setupdrawslab_eax;
				ebx2 += edx;
			} while (--ecx2);

			edi++;
		}

		if ((edi & 2) && edi < a - 1)
		{
			intptr_t edi2 = edi;
			int32_t ebx2 = ebx;
			uint32_t ecx2 = ecx;
			do
			{
				char c = *(char*)(esi + ((uint32_t)ebx2 >> 16));
				col16_t col;
				col.a = col.b = *(char*)(setupdrawslab_ebx + c);
				*(short*)edi2 = col.m;
				edi2 += setupdrawslab_eax;
				ebx2 += edx;
			} while (--ecx2);

			edi += 2;
		}

		while (edi < a - 3)
		{
			intptr_t edi2 = edi;
			int32_t ebx2 = ebx;
			uint32_t ecx2 = ecx;
			do
			{
				char c = *(char*)(esi + ((uint32_t)ebx2 >> 16));
				col32_t col;
				col.a = col.b = col.c = col.d = *(char*)(setupdrawslab_ebx + c);
				*(uint32_t*)edi2 = col.m;
				edi2 += setupdrawslab_eax;
				ebx2 += edx;
			} while (--ecx2);
			edi += 4;
		}

		if (edi < a - 1)
		{
			intptr_t edi2 = edi;
			int32_t ebx2 = ebx;
			uint32_t ecx2 = ecx;
			do
			{
				char c = *(char*)(esi + ((uint32_t)ebx2 >> 16));
				col16_t col;
				col.a = col.b = *(char*)(setupdrawslab_ebx + c);
				*(short*)edi2 = col.m;
				edi2 += setupdrawslab_eax;
				ebx2 += edx;
			} while (--ecx2);

			edi += 2;
		}
		if (edi != a)
		{
			intptr_t edi2 = edi;
			int32_t ebx2 = ebx;
			uint32_t ecx2 = ecx;
			do
			{
				char c = *(char*)(esi + ((uint32_t)ebx2 >> 16));
				*(char*)edi2 = *(char*)(setupdrawslab_ebx + c);
				edi2 += setupdrawslab_eax;
				ebx2 += edx;
			} while (--ecx2);
		}
	}
}
