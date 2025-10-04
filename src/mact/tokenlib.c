#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "compat.h"
#include "types.h"
#include "develop.h"
#include "file_lib.h"
#include "tokenlib.h"
#include "_tokenli.h"
#include "linklist.h"
#include "util_lib.h"

static token_t *tokenfiles[MAXTOKENFILES];
char token[128];
static boolean tokenlibrarycalled = false;

int TOKEN_GetLineNumber(int tokenhandle)
{
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_GetLineNumber: not a valid tokenhandle\n");

    return TOKENFILE(tokenhandle, scriptline);
}

void TOKEN_Reset(int tokenhandle)
{
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_Reset: not a valid tokenhandle\n");

    TOKENFILE(tokenhandle, script_p) = TOKENFILE(tokenhandle, scriptbuffer);
    TOKENFILE(tokenhandle, scriptline) = 1;
    TOKENFILE(tokenhandle, tokenready) = false;
}

boolean SkipToEOL(byte **script_p, byte *scriptend_p)
{
    while (**script_p != TOKENEOL)
    {
        if (*script_p >= scriptend_p)
            return true;
        (*script_p)++;
    }

    return false;
}

void TOKEN_CheckOverflow(int tokenhandle)
{
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_CheckOverflow: not a valid tokenhandle\n");

    if (TOKENFILE(tokenhandle, script_p) >= TOKENFILE(tokenhandle, scriptend_p))
        Error("End of token file reached prematurely reading %s\n", TOKENFILE(tokenhandle, scriptfilename));
}

void TOKEN_SkipWhiteSpace(int tokenhandle, boolean crossline)
{
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_SkipWhiteSpace: not a valid tokenhandle\n");

    TOKEN_CheckOverflow(tokenhandle);

    while (*TOKENFILE(tokenhandle, script_p) <= TOKENSPACE)
    {
        if (*TOKENFILE(tokenhandle, script_p)++ == TOKENEOL)
        {
            if (!crossline)
                Error("Line %i is incomplete\nin file %s\n", TOKENFILE(tokenhandle, scriptline), TOKENFILE(tokenhandle, scriptfilename));
            TOKENFILE(tokenhandle, scriptline)++;
        }
        TOKEN_CheckOverflow(tokenhandle);
    }
}

void TOKEN_SkipNonToken(int tokenhandle, boolean crossline)
{
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_SkipNonToken: not a valid tokenhandle\n");

    while (1)
    {
        TOKEN_SkipWhiteSpace(tokenhandle, crossline);
        if (*TOKENFILE(tokenhandle, script_p) == TOKENCOMMENT || *TOKENFILE(tokenhandle, script_p) == TOKENCOMMENT2)
            SkipToEOL(&TOKENFILE(tokenhandle, script_p), TOKENFILE(tokenhandle, scriptend_p));
        else
            break;
    }
}

boolean TOKEN_TokenAvailable(int32 tokenhandle, boolean crossline)
{
    byte* p;
    boolean end;
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_TokenAvailable: not a valid tokenhandle\n");

    p = TOKENFILE(tokenhandle, script_p);
    if (p >= TOKENFILE(tokenhandle, scriptend_p))
        return false;

    while (1)
    {
        if (*p <= TOKENSPACE)
        {
            if (*p == TOKENEOL && !crossline)
                return false;
            p++;
            if (p >= TOKENFILE(tokenhandle, scriptend_p))
                return false;
        }
        else if (*p == TOKENCOMMENT || *p == TOKENCOMMENT2)
        {
            end = SkipToEOL(&p, TOKENFILE(tokenhandle, scriptend_p));
            if (end == true)
                return false;
        }
        else
            break;
    }

    return true;
}

boolean TOKEN_CommentAvailable(int32 tokenhandle, boolean crossline)
{
    byte* p;
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_CommentAvailable: not a valid tokenhandle\n");

    p = TOKENFILE(tokenhandle, script_p);
    if (p >= TOKENFILE(tokenhandle, scriptend_p))
        return false;

    while (1)
    {
        if (*p <= TOKENSPACE)
        {
            if (*p == TOKENEOL && !crossline)
                return false;
            p++;
            if (p >= TOKENFILE(tokenhandle, scriptend_p))
                return false;
        }
        else
            break;
    }

    return true;
}

void TOKEN_UnGet(int32 tokenhandle)
{
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_UnGet: not a valid tokenhandle\n");

    TOKENFILE(tokenhandle, tokenready) = true;
}

void TOKEN_Get(int32 tokenhandle, boolean crossline)
{
    byte* p;
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_Get: not a valid tokenhandle\n");
    if (TOKENFILE(tokenhandle, tokenready))
    {
        TOKENFILE(tokenhandle, tokenready) = false;
        return;
    }
    TOKEN_SkipNonToken(tokenhandle, crossline);
    p = token;
    while (*TOKENFILE(tokenhandle, script_p) > TOKENSPACE && *TOKENFILE(tokenhandle, script_p) != TOKENCOMMENT
        && *TOKENFILE(tokenhandle, script_p) != TOKENCOMMENT2)
    {
        *p++ = *TOKENFILE(tokenhandle, script_p)++;
        if (TOKENFILE(tokenhandle, script_p) == TOKENFILE(tokenhandle, scriptend_p))
            break;
        if (p == &token[128])
            Error("Token too large on line %i\nin file %s\n", TOKENFILE(tokenhandle, scriptline), TOKENFILE(tokenhandle, scriptfilename));
    }
    *p = TOKENNULL;
}

void TOKEN_GetLine(int32 tokenhandle, boolean crossline)
{
    byte* p;
    int len;
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_GetLine: not a valid tokenhandle\n");
    if (TOKENFILE(tokenhandle, tokenready))
    {
        TOKENFILE(tokenhandle, tokenready) = false;
        return;
    }
    TOKEN_SkipNonToken(tokenhandle, crossline);
    p = TOKENFILE(tokenhandle, script_p);
    SkipToEOL(&TOKENFILE(tokenhandle, script_p), TOKENFILE(tokenhandle, scriptend_p));
    len = TOKENFILE(tokenhandle, script_p) - p;
    if (len < 127)
    {
        memcpy(token, p, len);
        token[len] = TOKENNULL;
    }
    else
        Error("Token too large on line %i\nin file %s\n", TOKENFILE(tokenhandle, scriptline), TOKENFILE(tokenhandle, scriptfilename));
}

void TOKEN_GetRaw(int32 tokenhandle)
{
    byte* p;
    int len;
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_GetRaw: not a valid tokenhandle\n");
    if (TOKENFILE(tokenhandle, tokenready))
    {
        TOKENFILE(tokenhandle, tokenready) = false;
        return;
    }
    TOKEN_SkipWhiteSpace(tokenhandle, true);
    p = TOKENFILE(tokenhandle, script_p);
    SkipToEOL(&TOKENFILE(tokenhandle, script_p), TOKENFILE(tokenhandle, scriptend_p));
    len = TOKENFILE(tokenhandle, script_p) - p;
    if (len < 127)
    {
        memset(token, 0, 128);
        memcpy(token, p, len + 1);
    }
    else
        Error("Token too large on line %i\nin file %s\n", TOKENFILE(tokenhandle, scriptline), TOKENFILE(tokenhandle, scriptfilename));
}

boolean TOKEN_GetSpecific(int32 tokenhandle, char* string)
{
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_GetSpecific: not a valid tokenhandle\n");

    do
    {
        if (!TOKEN_TokenAvailable(tokenhandle, true))
            return false;

        TOKEN_Get(tokenhandle, true);
    } while (!strcmp(token, string));

    return true;
}

void TOKEN_GetInteger(int32 tokenhandle, int32* number, boolean crossline)
{
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_GetInteger: not a valid tokenhandle\n");
    TOKEN_Get(tokenhandle, crossline);
    *number = atoi(token);
}

void TOKEN_GetDouble(int32 tokenhandle, double* number, boolean crossline)
{
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_GetDouble: not a valid tokenhandle\n");
    TOKEN_Get(tokenhandle, crossline);
    *number = strtod(token, NULL);
}

int32 TOKEN_LinesInFile(int32 tokenhandle)
{
    token_t bt;
    int32 lines;
    if (!tokenfiles[tokenhandle])
        Error("TOKEN_LinesInFile: not a valid tokenhandle\n");
    lines = 0;
    memcpy(&bt, tokenfiles[tokenhandle], sizeof(token_t));
    TOKEN_Reset(tokenhandle);
    while (TOKEN_TokenAvailable(tokenhandle, true))
    {
        TOKEN_GetLine(tokenhandle, true);
        lines++;
    }
    memcpy(tokenfiles[tokenhandle], &bt, sizeof(token_t));

    return lines;
}

int32 TOKEN_New(void)
{
    int i;
    if (!tokenlibrarycalled)
    {
        tokenlibrarycalled = true;
        memset(tokenfiles, 0, sizeof(tokenfiles));
    }
    for (i = 0; i < 20; i++)
    {
        if (!tokenfiles[i])
        {
            tokenfiles[i] = SafeMalloc(sizeof(token_t));
            return i;
        }
    }
    Error("TOKEN_New: No free token file slots available\n");
    return -1;
}

void TOKEN_Free(int32 tokenhandle)
{
    if (!tokenfiles[tokenhandle])
        return;

    if (TOKENFILE(tokenhandle, scriptbuffer))
        SafeFree(TOKENFILE(tokenhandle, scriptbuffer));
    SafeFree(tokenfiles[tokenhandle]);
    tokenfiles[tokenhandle] = NULL;
}

int32 TOKEN_Parse(const char* data, int32 length, const char* name)
{
    int32 tokenhandle;

    tokenhandle = TOKEN_New();
    TOKENFILE(tokenhandle, scriptbuffer) = SafeMalloc(length);
    memcpy(TOKENFILE(tokenhandle, scriptbuffer), data, length);
    TOKEN_Reset(tokenhandle);
    TOKENFILE(tokenhandle, scriptend_p) = TOKENFILE(tokenhandle, script_p) + length;
    strcpy(TOKENFILE(tokenhandle, scriptfilename), name);
    return tokenhandle;
}

int32 TOKEN_Load(const char* filename)
{
    void* ptr;
    int32 len;
    int32 tokenhandle;
    len = LoadFile(filename, &ptr);
    tokenhandle = TOKEN_Parse(ptr, len, filename);
    SafeFree(ptr);
    return tokenhandle;
}
