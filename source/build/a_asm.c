#include "compat.h"

extern int32_t asm1, asm2, asm3, ams4;
extern int32_t reciptable[];
extern union {
	uint32_t i;
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

static int32_t setuphlineasm4_eax, setuphlineasm4_ebx;
void setuphlineasm4(int32_t eax, int32_t ebx)
{
	uint32_t a = (uint32_t)eax << sethlinesizes_al;
	a |= (uint32_t)eax >> (32 - sethlinesizes_al);
	setuphlineasm4_eax = a;
	ebx &= ~255;
	ebx |= a & 255;
	setuphlineasm4_ebx = ebx;
}

static uint64_t hlineasm4_sub;

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
			*(char*)edi = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a)];
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
			char bh = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a)];
			esi -= asm1;
			edx -= asm2;
			a = (uint32_t)esi >> sethlinesizes_al_n;
			a <<= sethlinesizes_bl;
			a |= (uint32_t)edx >> sethlinesizes_bl_n;
			char bl = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a)];
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
			hlineasm4_sub = asm1;
			uint32_t h = asm2;
			h &= ~255;
			h |= asm1 & 255;
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
			col.d = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a)];

			a = (uint32_t)crd.h << sethlinesizes_bl;
			a |= (uint32_t)crd.h >> (32 - sethlinesizes_bl);
			a &= sethlinesizes_mask;
			crd.m -= hlineasm4_sub;

			col.c = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a)];

			a = (uint32_t)crd.h << sethlinesizes_bl;
			a |= (uint32_t)crd.h >> (32 - sethlinesizes_bl);
			a &= sethlinesizes_mask;
			crd.m -= hlineasm4_sub;

			col.b = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a)];

			a = (uint32_t)crd.h << sethlinesizes_bl;
			a |= (uint32_t)crd.h >> (32 - sethlinesizes_bl);
			a &= sethlinesizes_mask;
			crd.m -= hlineasm4_sub;

			col.a = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a)];

			ebp -= 4;
			if (ebp >= 0)
			{
				*(uint32_t*)edi = col.m;
				continue;
			}

			if (ebp & 2)
			{
				*(uint32_t*)(edi + 2) = col.m >> 16;
				col.d = col.b;
				edi -= 2;
			}
			if (ebp & 1)
			{
				*(uint32_t*)(edi + 3) = col.d;
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
			*(char*)edi = setpalookupaddress_eax[*(char*)(sethlinesizes_ecx + a)];
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
		*(char*)edi = *(char*)(ebx + (char*)(esi + a));
	} while (--ecx);

	return edx;
}

int32_t mvlineasm1(int32_t eax, intptr_t ebx, int32_t ecx, int32_t edx, intptr_t esi, intptr_t edi)
{
	do
	{
		uint32_t a = (uint32_t)edx >> setupmvlineasm_al;
		edx += eax;
		char c = (char*)(esi + a);
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

int32_t tvlineasm1(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, int32_t edi)
{
	ecx++;
	do
	{
		uint32_t a = (uint32_t)edx >> setuptvlineasm_al;
		edx += eax;
		char c = (char*)(esi + a);
		if (c != 255)
		{
			char s = *(char*)edi;
			c = *(char*)(fixtransluscence_eax + (s << trans_s2) + (c << trans_s1));
			*(char*)edi = *(char*)(ebx + c);
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
	add1 |= (uint64_t)(vince[2] + (va & 0x1ff)) << 32;
	uint16_t add2_l = vb >> 16;
	uint32_t add2_h = vince[2] + (va & 0x1ff);

	va = ((uint32_t)vplce[3] << setupvlineasm_al_n) | (((uint32_t)vplce[3] >> (32 - setupvlineasm_al_n)));
	vb = ((uint32_t)vplce[1] << setupvlineasm_al_n) | (((uint32_t)vplce[1] >> (32 - setupvlineasm_al_n)));
	texcoord64_t crd;
	crd.l = (va & 0xffff0000) | (vb >> 16);
	crd.h = vplce[2] + (va & 0x1ff);
	uint32_t crd_h2 = vplce[0] + (vb & 0x1ff);

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
		*(char*)&o1 = *(char*)(bufplce[0] + o1);
		*(char*)&o2 = *(char*)(bufplce[0] + o2);
		*(char*)&o3 = *(char*)(bufplce[0] + o3);

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
