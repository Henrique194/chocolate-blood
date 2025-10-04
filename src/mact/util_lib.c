#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <sys\types.h>
#include "compat.h"
#include "system.h"
#include "types.h"
#include "develop.h"
#include "fixed.h"
#include "_util_lb.h"
#include "util_lib.h"
#include <malloc.h>
#include <io.h>

static void(*ShutDown)(void);

void RegisterShutdownFunction(void (*shutdown) (void))
{
    ShutDown = shutdown;
}

void Error(const char* error, ...)
{
    va_list va;
    static boolean inerror = false;
    if (!inerror)
    {
        inerror = true;
        if (ShutDown)
            ShutDown();
        va_start(va, error);
        vprintf(error, va);
        va_end(va);
        sys_printf("\n");
        exit(1);
    }
}

char CheckParm(const char* check)
{
    char i;
    char t[] = "\\-/";
    char buf[128];
    char* s;
    for (i = 1; i < sys_argc; i++)
    {
        strcpy(buf, sys_argv[i]);
        s = strtok(buf, t);
        if (!s)
            continue;

        if (!stricmp(check, s))
            return i;
    }

    return 0;
}

int32 ParseHex(char* hex)
{
    int val = 0;
    char* s = hex;
    while (*s)
    {
        val <<= 4;
        if (*s >= '0' && *s <= '9')
            val += *s - '0';
        else if (*s >= 'a' && *s <= 'f')
            val += *s - 'a' + 10;
        else if (*s >= 'A' && *s <= 'F')
            val += *s - 'A' + 10;
        else
            Error("Bad hex number: %s", hex);

        s++;
    }

    return val;
}

int32 ParseNum(char* str)
{
    if (*str == '$')
        return ParseHex(str + 1);
    if (str[0] == '0' && str[1] == 'x')
        return ParseHex(str + 2);

    return atol(str);
}


int16 MotoShort(int16 l)
{
    char a = l & 255;
    char b = (l >> 8) & 255;

    return (a << 8) + b;
}

int16 IntelShort(int16 l)
{
    return l;
}

int32 MotoLong(int32 l)
{
    char a = l & 255;
    char b = (l >> 8) & 255;
    char c = (l >> 16) & 255;
    char d = (l >> 24) & 255;

    return (a << 24) + (b << 16) + (c << 8) + d;
}

int32 IntelLong(int32 l)
{
    return l;
}

static int32 Width;
static int32 (*Comp)(char *a, char *b);
static int32 (*Switch)(char *a, char *b);
static char* Base;

static void newsift_down(int a, int b);

void HeapSort(char* base, int32 nel, int32 width, int32(*compare)(), void (*switcher)())
{
    static int v1, v2, v3;

    Width = width;
    Comp = compare;
    Switch = switcher;
    v2 = nel * Width;
    Base = base - Width;

    for (v1 = ((v2 / Width) / 2) * Width; v1 >= Width; v1 -= Width)
    {
        newsift_down(v1, v2);
    }

    v3 = Width * 2;

    for (v1 = v2; v1 >= v3;)
    {
        Switch(base, Base + v1);
        newsift_down(Width, v1 -= Width);
    }
}

static void newsift_down(int a, int b)
{
    int v1;
    while (1)
    {
        v1 = a * 2;
        if (v1 > b)
            break;

        if (v1 + Width <= b && Comp(Base + v1 + Width, Base + v1) > 0)
            v1 += Width;

        if (Comp(Base + a, Base + v1) >= 0)
            break;

        Switch(Base + a, Base + v1);

        a = v1;
    }
}

void* SafeMalloc(int32 size)
{
    char* p;

    p = malloc(size);
    if (!p)
        Error("SafeMalloc failure for %lu bytes", size);
    return p;
}

int32 SafeMallocSize(void* ptr)
{
    return _msize(ptr);
}

void SafeRealloc(void** ptr, int32 newsize)
{
    *ptr = realloc(*ptr, newsize);
    if (!*ptr)
        Error("SafeRealloc failure for %lu bytes", newsize);
}

void SafeFree(void* ptr)
{
    if (!ptr)
        Error("SafeFree : Tried to free a freed pointer");

    free(ptr);
}

