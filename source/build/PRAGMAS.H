// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.
// This file has been modified from Ken Silverman's original release

static inline int32_t sqr(int32_t eax)
{
	return eax * eax;
}

static inline int32_t scale(int32_t eax, int32_t edx, int32_t ecx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m /= ecx;
	return (int32_t)m;
}

static inline int32_t mulscale(int32_t eax, int32_t edx, int32_t ecx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> (ecx & 31));
}

static inline int32_t mulscale1(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 1);
}

static inline int32_t mulscale2(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 2);
}

static inline int32_t mulscale3(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 3);
}

static inline int32_t mulscale4(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 4);
}

static inline int32_t mulscale5(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 5);
}

static inline int32_t mulscale6(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 6);
}

static inline int32_t mulscale7(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 7);
}

static inline int32_t mulscale8(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 8);
}

static inline int32_t mulscale9(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 9);
}

static inline int32_t mulscale10(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 10);
}

static inline int32_t mulscale11(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 11);
}

static inline int32_t mulscale12(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 12);
}

static inline int32_t mulscale13(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 13);
}

static inline int32_t mulscale14(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 14);
}

static inline int32_t mulscale15(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 15);
}

static inline int32_t mulscale16(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 16);
}

static inline int32_t mulscale17(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 17);
}

static inline int32_t mulscale18(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 18);
}

static inline int32_t mulscale19(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 19);
}

static inline int32_t mulscale20(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 20);
}

static inline int32_t mulscale21(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 21);
}

static inline int32_t mulscale22(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 22);
}

static inline int32_t mulscale23(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 23);
}

static inline int32_t mulscale24(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 24);
}

static inline int32_t mulscale25(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 25);
}

static inline int32_t mulscale26(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 26);
}

static inline int32_t mulscale27(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 27);
}

static inline int32_t mulscale28(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 28);
}

static inline int32_t mulscale29(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 29);
}

static inline int32_t mulscale30(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 30);
}

static inline int32_t mulscale31(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 31);
}

static inline int32_t mulscale32(int32_t eax, int32_t edx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	return (int32_t)(m >> 32);
}

static inline int32_t dmulscale(int32_t eax, int32_t edx, int32_t esi, int32_t edi, int32_t ecx)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> (ecx & 31));
}

static inline int32_t dmulscale1(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 1);
}

static inline int32_t dmulscale2(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 2);
}

static inline int32_t dmulscale3(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 3);
}

static inline int32_t dmulscale4(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 4);
}

static inline int32_t dmulscale5(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 5);
}

static inline int32_t dmulscale6(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 6);
}

static inline int32_t dmulscale7(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 7);
}

static inline int32_t dmulscale8(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 8);
}

static inline int32_t dmulscale9(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 9);
}

static inline int32_t dmulscale10(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 10);
}

static inline int32_t dmulscale11(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 11);
}

static inline int32_t dmulscale12(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 12);
}

static inline int32_t dmulscale13(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 13);
}

static inline int32_t dmulscale14(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 14);
}

static inline int32_t dmulscale15(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 15);
}

static inline int32_t dmulscale16(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 16);
}

static inline int32_t dmulscale17(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 17);
}

static inline int32_t dmulscale18(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 18);
}

static inline int32_t dmulscale19(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 19);
}

static inline int32_t dmulscale20(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 20);
}

static inline int32_t dmulscale21(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 21);
}

static inline int32_t dmulscale22(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 22);
}

static inline int32_t dmulscale23(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 23);
}

static inline int32_t dmulscale24(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 24);
}

static inline int32_t dmulscale25(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 25);
}

static inline int32_t dmulscale26(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 26);
}

static inline int32_t dmulscale27(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 27);
}

static inline int32_t dmulscale28(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 28);
}

static inline int32_t dmulscale29(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 29);
}

static inline int32_t dmulscale30(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 30);
}

static inline int32_t dmulscale31(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 31);
}

static inline int32_t dmulscale32(int32_t eax, int32_t edx, int32_t esi, int32_t edi)
{
	int64_t m = (int64_t)eax * (int64_t)edx;
	m += (int64_t)esi * (int64_t)edi;
	return (int32_t)(m >> 32);
}

static inline int32_t tmulscale11(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f)
{
	int64_t m = (int64_t)a * (int64_t)b;
	m += (int64_t)c * (int64_t)d;
	m += (int64_t)e * (int64_t)f;
	return (int32_t)(m >> 11);
}

static inline int32_t divscale(int32_t eax, int32_t ebx, int32_t ecx)
{
	int64_t m = (int64_t)eax << (ecx & 31);
	return (int32_t)(m / ebx);
}

static inline int32_t divscale1(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 1;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale2(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 2;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale3(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 3;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale4(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 4;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale5(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 5;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale6(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 6;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale7(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 7;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale8(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 8;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale9(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 9;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale10(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 10;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale11(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 11;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale12(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 12;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale13(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 13;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale14(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 14;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale15(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 15;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale16(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 16;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale17(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 17;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale18(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 18;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale19(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 19;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale20(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 20;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale21(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 21;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale22(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 22;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale23(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 23;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale24(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 24;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale25(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 25;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale26(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 26;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale27(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 27;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale28(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 28;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale29(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 29;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale30(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 30;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale31(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 31;
	return (int32_t)(m / ebx);
}

static inline int32_t divscale32(int32_t eax, int32_t ebx)
{
	int64_t m = (int64_t)eax << 32;
	return (int32_t)(m / ebx);
}

static inline uint8_t readpixel(char* ptr)
{
	return *ptr;
}

static inline void drawpixel(char* ptr, char data)
{
	*ptr = data;
}

static inline void drawpixels(char* ptr, short data)
{
	*(short*)ptr = data;
}

static inline void drawpixelses(char* ptr, int32_t data)
{
	*(int32_t*)ptr = data;
}

static inline void clearbuf(char *ptr, uint32_t cnt, uint32_t val)
{
	while (cnt--)
	{
		*(uint32_t*)ptr = val;
		ptr += 4;
	}
}

static inline void clearbufbyte(char *ptr, uint32_t cnt, uint32_t val)
{
	if (cnt < 4)
	{
		if (cnt & 1)
			*ptr++ = val;
		cnt >>= 1;
		while (cnt--)
		{
			*(uint16_t*)ptr = val;
			ptr += 2;
		}
	}
	else
	{
		if ((intptr_t)ptr & 1)
		{
			*ptr++ = val;
			cnt--;
		}
		if ((intptr_t)ptr & 2)
		{
			*(uint16_t*)ptr = val;
			ptr += 2;
			cnt -= 2;
		}
		uint32_t cnt2 = cnt >> 2;
		while (cnt2--)
		{
			*(uint32_t*)ptr = val;
			ptr += 4;
		}
		if (cnt & 2)
		{
			*(uint16_t*)ptr = val;
			ptr += 2;
		}
		if (cnt & 1)
		{
			*ptr++ = val;
			cnt--;
		}
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

static inline void copybufbyte(char *src, char *dst, uint32_t cnt)
{
	if (cnt < 4)
	{
		if (cnt & 1)
			*dst++ = *src++;
		cnt >>= 1;
		while (cnt--)
		{
			*(uint16_t*)dst = *(uint16_t*)src;
			dst += 2; src += 2;
		}
	}
	else
	{
		if ((intptr_t)dst & 1)
		{
			*dst++ = *src++;
			cnt--;
		}
		if ((intptr_t)dst & 2)
		{
			*(uint16_t*)dst = *(uint16_t*)src;
			dst += 2; src += 2;
			cnt -= 2;
		}
		uint32_t cnt2 = cnt >> 2;
		while (cnt2--)
		{
			*(uint32_t*)dst = *(uint32_t*)src;
			dst += 4; src += 4;
		}
		if (cnt & 2)
		{
			*(uint16_t*)dst = *(uint16_t*)src;
			dst += 2; src += 2;
		}
		if (cnt & 1)
		{
			*dst++ = *src++;
			cnt--;
		}
	}
}

static inline void copybufreverse(char *src, char *dst, uint32_t ecx)
{
	if (ecx & 1)
		*dst++ = *src--;
	ecx >>= 1;

	if (ecx & 1)
	{
		*(short*)dst = (src[-1] << 8) | src[0];
		dst += 2; src -= 2;
	}
	ecx >>= 1;

	while (ecx)
	{
		*(int32_t*)dst = (src[-3] << 24) | (src[-2] << 16) | (src[-1] << 8) | src[0];
		dst += 4; src -= 4;

		ecx--;
	}
}

static inline void qinterpolatedown16(int32_t *dst, uint32_t cnt, int32_t base, int32_t step)
{
	uint32_t cnt2 = (cnt >> 1);
	if (cnt2)
	{
		do
		{
			dst[0] = base >> 16;
			base += step;
			dst[1] = base >> 16;
			base += step;
			dst += 2;
		} while (--cnt2);

		if ((cnt & 1) == 0)
			return;
	}
	*dst = base >> 16;
}

static inline void qinterpolatedown16short(int16_t *dst, uint32_t cnt, int32_t base, int32_t step)
{
	if (!cnt)
		return;
	if ((intptr_t)dst & 2)
	{
		*dst++ = base >> 16;
		base += step;
		if (!--cnt)
			return;
	}

	cnt -= 2;
	if (cnt < (uint32_t)-2)
	{
		do
		{
			uint32_t val = base >> 16;
			base += step;
			val += base & 0xffff0000;
			base += step;

			*(int32_t*)dst = val;
			dst += 2;
			cnt -= 2;
		} while (cnt < (uint32_t)-2);

		if ((cnt & 1) == 0)
			return;
	}
	*dst++ = base >> 16;
}

static inline int32_t mul3(int32_t a)
{
	return a * 3;
}

static inline int32_t mul5(int32_t a)
{
	return a * 5;
}

static inline int32_t mul9(int32_t a)
{
	return a * 9;
}

static inline int32_t klabs(int32_t a)
{
	if (a < 0)
		return -a;
	return a;
}

static inline int32_t ksgn(int32_t a)
{
	return (a > 0) - (a < 0);
}

static inline void swapchar(char* a, char* b)
{
	char t = *a;
	*a = *b;
	*b = t;
}

static inline void swapshort(short* a, short* b)
{
	short t = *a;
	*a = *b;
	*b = t;
}

static inline void swaplong(int32_t* a, int32_t* b)
{
	int32_t t = *a;
	*a = *b;
	*b = t;
}

static inline void swapchar2(char* a, char* b, uint32_t c)
{
	char t1 = a[0];
	char t2 = a[1];
	a[0] = b[0];
	a[1] = b[c];
	b[0] = t1;
	b[c] = t2;
}
