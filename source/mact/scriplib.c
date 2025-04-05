#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "compat.h"
#include "types.h"
#include "develop.h"
#include "file_lib.h"
#include "scriplib.h"
#include "_scrplib.h"
#include "linklist.h"
#include "util_lib.h"
#include "tokenlib.h"

static script_t *scriptfiles[MAXSCRIPTFILES];
static boolean scriptlibrarycalled = false;

boolean SCRIPT_HandleValid(int32 scripthandle)
{
    return scriptfiles[scripthandle] != NULL;
}

int32 SCRIPT_New(void)
{
    int i;
    if (!scriptlibrarycalled)
    {
        scriptlibrarycalled = true;
        memset(scriptfiles, 0, sizeof(scriptfiles));
    }

    for (i = 0; i < MAXSCRIPTFILES; i++)
    {
        if (!SCRIPT_HandleValid(i))
        {
            scriptfiles[i] = SafeMalloc(sizeof(script_t));
            return i;
        }
    }
    Error("SCRIPT_New: No free script file slots available\n");
    return -1;
}

void SCRIPT_Delete(int32 scripthandle)
{
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_Delete not a valid scripthandle\n");
    SafeFree(scriptfiles[scripthandle]);
    scriptfiles[scripthandle] = NULL;
}

int32 SCRIPT_Init(char* name)
{
    int32 scripthandle;

    scripthandle = SCRIPT_New();

    LL_CreateNewLinkedList(SCRIPT(scripthandle, script), ScriptSectionType, prevsection, nextsection);
    LL_CreateNewLinkedList(SCRIPT(scripthandle, scriptlines), ScriptLineType, prevline, nextline);
    SCRIPT(scripthandle, lastsection) = NULL;
    strcpy(SCRIPT(scripthandle, scriptfilename), name);

    return scripthandle;
}

void SCRIPT_FreeSection(ScriptSectionType *section)
{
    ScriptEntryType* e, *e2;
    
    e = section->entries->nextentry;

    while (e != section->entries)
    {
        e2 = e;
        e = e->nextentry;

        SafeFree(e2->name);
        SafeFree(e2->value);
        LL_RemoveNode(e2, nextentry, preventry);
        SafeFree(e2);
    }
    SafeFree(section->entries);
    SafeFree(section->name);
    section->entries = NULL;
}

void SCRIPT_Free(int32 scripthandle)
{
    ScriptSectionType* s, *s2;
    ScriptLineType* l, * l2;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_Free not a valid scripthandle\n");

    s = SCRIPT(scripthandle, script)->nextsection;
    while (s != SCRIPT(scripthandle, script))
    {
        s2 = s;
        s = s->nextsection;
        SCRIPT_FreeSection(s2);
        LL_RemoveNode(s2, nextsection, prevsection);
        SafeFree(s2);
    }
    l = SCRIPT(scripthandle, scriptlines)->nextline;
    while (l != SCRIPT(scripthandle, scriptlines))
    {
        l2 = l;
        if (l->type == linetype_comment)
            SafeFree(l->ptr);
        l = l->nextline;
        LL_RemoveNode(l2, nextline, prevline);
        SafeFree(l2);
    }
    SafeFree(SCRIPT(scripthandle, scriptlines));
    SafeFree(SCRIPT(scripthandle, script));
    SCRIPT_Delete(scripthandle);
}

int32 SCRIPT_Parse(char* data, int32 length, char* name)
{
    boolean end = 0;
    int tokenhandle;
    int scripthandle;

    tokenhandle = TOKEN_Parse(data, length, name);

    scripthandle = SCRIPT_Init(name);
    TOKEN_Reset(tokenhandle);


    while (!end)
    {
        if (!TOKEN_CommentAvailable(tokenhandle, true))
            end = true;
        else
        {
            TOKEN_GetRaw(tokenhandle);
            SCRIPT_DecodeToken(scripthandle, token);
        }
    }

    TOKEN_Free(tokenhandle);

    return scripthandle;
}

int32 SCRIPT_Load(char* filename)
{
    int len;
    int scripthandle;
    void *ptr;
    if (SafeFileExists(filename))
    {
        len = LoadFile(filename, &ptr);
        scripthandle = SCRIPT_Parse(ptr, len, filename);
        SafeFree(ptr);
    }
    else
        scripthandle = SCRIPT_Init(filename);

    return scripthandle;
}

void SafeWriteString(int32 handle, char* string)
{
    int len;

    len = strlen(string);
    SafeWrite(handle, string, len);
}

void SCRIPT_Save(int32 scripthandle, char* filename)
{
    char buf[160];
    ScriptLineType* l;
    int handle;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_Save not a valid scripthandle\n");

    handle = SafeOpenWrite(filename, filetype_text);

    l = SCRIPT(scripthandle, scriptlines)->nextline;
    while (l != SCRIPT(scripthandle, scriptlines))
    {
        switch (l->type)
        {
            case linetype_comment:
                sprintf(buf, "%c%s%c", SCRIPTCOMMENT, (char*)l->ptr, SCRIPTEOL);
                SafeWriteString(handle, buf);
                break;
            case linetype_section:
            {
                ScriptSectionType* s;
                s = (ScriptSectionType*)l->ptr;
                sprintf(buf, "%c%s%c%c", SCRIPTSECTIONSTART, s->name, SCRIPTSECTIONEND, SCRIPTEOL);
                SafeWriteString(handle, buf);
                break;
            }
            case linetype_entry:
            {
                ScriptEntryType* e;
                e = (ScriptEntryType*)l->ptr;
                sprintf(buf, "%s %c %s%c", e->name, SCRIPTENTRYSEPARATOR, e->value, SCRIPTEOL);
                SafeWriteString(handle, buf);
                break;
            }
        }
        l = l->nextline;
    }
    SafeClose(handle);
}

ScriptLineType* SCRIPT_AddLine
(
    ScriptLineType* root,
    int32 type,
    void* ptr
)
{
    ScriptLineType* l = SafeMalloc(sizeof(ScriptLineType));

    l->type = type;
    l->ptr = ptr;

    LL_AddNode(root, l, nextline, prevline);

    return l;
}

ScriptSectionType* SCRIPT_SectionExists
(
    int32 scripthandle,
    char* sectionname
)
{
    ScriptSectionType* s;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_SectionExists not a valid scripthandle\n");

    s = SCRIPT(scripthandle, script)->nextsection;
    while (s != SCRIPT(scripthandle, script))
    {
        if (!strcmpi(sectionname, s->name))
        {
            return s;
        }
        s = s->nextsection;
    }
    return NULL;
}

void SCRIPT_AddSection(int32 scripthandle, char* sectionname)
{
    ScriptSectionType* s;
    int len;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_AddSection not a valid scripthandle\n");

    if (!SCRIPT_SectionExists(scripthandle, sectionname))
    {
        s = SafeMalloc(sizeof(ScriptSectionType));
        LL_AddNode(SCRIPT(scripthandle, script), s, nextsection, prevsection);
        len = strlen(sectionname) + 1;

        s->name = SafeMalloc(len);
        memcpy(s->name, sectionname, len);
        s->lastline = SCRIPT_AddLine(SCRIPT(scripthandle, scriptlines), linetype_section, s);

        LL_CreateNewLinkedList(s->entries, ScriptEntryType, nextentry, preventry);
    }

    SCRIPT(scripthandle, lastsection) = SCRIPT_SectionExists(scripthandle, sectionname);
}

ScriptEntryType* SCRIPT_EntryExists
(
    ScriptSectionType* section,
    char* entryname
)
{
    ScriptEntryType* e;
    e = section->entries->nextentry;
    while (e != section->entries)
    {
        if (!strcmpi(entryname, e->name))
        {
            return e;
        }
        e = e->nextentry;
    }
    return NULL;
}

void SCRIPT_AddEntry
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    char* entryvalue
)
{
    ScriptSectionType* s;
    ScriptEntryType* e;
    ScriptLineType* l;
    boolean exist;
    int len;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_AddEntry not a valid scripthandle\n");

    s = SCRIPT_SectionExists(scripthandle, sectionname);

    if (!s)
    {
        SCRIPT_AddSection(scripthandle, sectionname);
        s = SCRIPT_SectionExists(scripthandle, sectionname);
    }

    exist = SCRIPT_EntryExists(s, entryname) != NULL;
    if (!exist)
    {
        e = SafeMalloc(sizeof(ScriptEntryType));
        LL_AddNode(s->entries, e, nextentry, preventry);
        e->name = NULL;
        e->value = NULL;
    }
    else
        e = SCRIPT_EntryExists(s, entryname);

    if (!e->name)
    {
        len = strlen(entryname) + 1;
        e->name = SafeMalloc(len);
        memcpy(e->name, entryname, len);
    }

    if (e->value)
        SafeFree(e->value);
    len = strlen(entryvalue) + 1;
    e->value = SafeMalloc(len);
    memcpy(e->value, entryvalue, len);
    if (!exist)
    {
        l = SCRIPT_AddLine(s->lastline->nextline, linetype_entry, e);
        s->lastline = l;
    }

    SCRIPT(scripthandle, lastsection) = SCRIPT_SectionExists(scripthandle, sectionname);
}

void SCRIPT_DecodeToken(int32 scripthandle, char* str)
{
    if (*str == SCRIPTSECTIONSTART)
    {
        char* p;
        str++;
        p = str;
        while (*str != SCRIPTSECTIONEND)
        {
            if (*str == SCRIPTNULL)
                Error("No matching bracket found for section %s in file %s\n", p, SCRIPT(scripthandle, scriptfilename));
            str++;
        }
        *str = SCRIPTNULL;
        SCRIPT_AddSection(scripthandle, p);
    }
    else if (*str == SCRIPTCOMMENT)
    {
        char* p;
        char* p2;
        int len;
        ScriptLineType* l;
        str++;
        p = str;
        while (*str >= SCRIPTSPACE)
        {
            str++;
        }
        len = (str - p) + 1;
        p2 = SafeMalloc(len);
        memset(p2, 0, len);
        if (len > 1)
            memcpy(p2, p, len - 1);

        if (SCRIPT(scripthandle, lastsection))
        {
            l = SCRIPT_AddLine(SCRIPT(scripthandle, lastsection)->lastline->nextline, linetype_comment, p2);
            SCRIPT(scripthandle, lastsection)->lastline = l;
        }
    }
    else
    {
        char* p;
        char* p2;
        p = str;
        while (*str > SCRIPTSPACE && *str != SCRIPTENTRYSEPARATOR)
        {
            str++;
        }

        if (*str == SCRIPTENTRYSEPARATOR)
        {
            *str = SCRIPTNULL;
            while (*str <= SCRIPTSPACE)
                str++;
        }
        else
        {
            *str = SCRIPTNULL;
            str++;
            while (*str <= SCRIPTSPACE)
                str++;

            if (*str != SCRIPTENTRYSEPARATOR)
                Error("No entry separator found for %s in %s\n", SCRIPT(scripthandle, lastsection)->name, SCRIPT(scripthandle, scriptfilename));

            str++;
            while (*str <= SCRIPTSPACE)
                str++;
        }
        p2 = str;
        while (*str >= SCRIPTSPACE)
        {
            str++;
        }
        *str = SCRIPTNULL;
        SCRIPT_AddEntry(scripthandle, SCRIPT(scripthandle, lastsection)->name, p, p2);
    }
}

int32 SCRIPT_NumberSections(int32 scripthandle)
{
    int num;
    ScriptSectionType* s;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_NumberSections not a valid scripthandle\n");

    s = SCRIPT(scripthandle, script)->nextsection;
    num = 0;
    while (s != SCRIPT(scripthandle, script))
    {
        num++;
        s = s->nextsection;
    }
    return num;
}

char* SCRIPT_Section(int32 scripthandle, int32 which)
{
    int num;
    ScriptSectionType* s;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_Section not a valid scripthandle\n");

    s = SCRIPT(scripthandle, script)->nextsection;
    num = 0;
    while (s != SCRIPT(scripthandle, script))
    {
        if (num == which)
            break;
        num++;
        s = s->nextsection;
    }

    if (s == SCRIPT(scripthandle, script))
        Error("There are only %ld sections, section %ld could not be found\n", SCRIPT_NumberSections(scripthandle), which);

    return s->name;
}

int32 SCRIPT_NumberEntries(int32 scripthandle, char* sectionname)
{
    int num;
    ScriptSectionType* s;
    ScriptEntryType* e;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_NumberEntries not a valid scripthandle\n");
    num = 0;
    s = SCRIPT_SectionExists(scripthandle, sectionname);
    if (s)
    {
        e = s->entries->nextentry;
        while (e != s->entries)
        {
            num++;
            e = e->nextentry;
        }
    }
    return num;
}

char* SCRIPT_Entry(int32 scripthandle, char* sectionname, int32 which)
{
    int num;
    ScriptSectionType* s;
    ScriptEntryType* e;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_Entry not a valid scripthandle\n");
    num = 0;
    s = SCRIPT_SectionExists(scripthandle, sectionname);
    e = s->entries->nextentry;
    while (e != s->entries)
    {
        if (num == which)
            break;
        num++;
        e = e->nextentry;
    }

    if (e == s->entries)
        Error("There are only %ld entries in section %s, entry %ld could not be found\n",
            SCRIPT_NumberEntries(scripthandle, sectionname), sectionname, which);
    return e->name;
}

char* SCRIPT_GetRaw(int32 scripthandle, char* sectionname, char* entryname)
{
    ScriptSectionType* s;
    ScriptEntryType* e;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_GetRaw not a valid scripthandle\n");

    char* value = NULL;
    s = SCRIPT_SectionExists(scripthandle, sectionname);
    if (s)
    {
        e = SCRIPT_EntryExists(s, entryname);
        if (e)
            value = e->value;
    }
    return value;
}

void SCRIPT_GetString
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    char* dest
)
{
    char* value;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_GetString not a valid scripthandle\n");
    value = SCRIPT_GetRaw(scripthandle, sectionname, entryname);
    if (value)
    {
        char *p = value;
        if (*p != SCRIPTSTRINGSEPARATOR)
            Error("SCRIPT_GetString: no %c found\n", SCRIPTSTRINGSEPARATOR);
        p++;
        value = p;
        while (*p != SCRIPTSTRINGSEPARATOR)
        {
            p++;
            if (p - value > 80)
                Error("SCRIPT_GetString: no %c found at end of string %s", SCRIPTSTRINGSEPARATOR, value);
        }
        memcpy(dest, value, p - value);
        dest[p - value] = SCRIPTNULL;
    }
}

void SCRIPT_GetDoubleString
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    char* dest1,
    char* dest2
)
{
    char* value;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_GetDoubleString not a valid scripthandle\n");
    value = SCRIPT_GetRaw(scripthandle, sectionname, entryname);
    if (value)
    {
        char* p = value;
        if (*p != SCRIPTSTRINGSEPARATOR)
            Error("SCRIPT_GetDoubleString: no %c found\n", SCRIPTSTRINGSEPARATOR);
        p++;
        value = p;
        while (*p != SCRIPTSTRINGSEPARATOR)
        {
            p++;
            if (p - value > 80)
                Error("SCRIPT_GetDoubleString: no %c found at end of string %s", SCRIPTSTRINGSEPARATOR, value);
        }
        memcpy(dest1, value, p - value);
        dest1[p - value] = SCRIPTNULL;

        p++;
        while (*p != SCRIPTSTRINGSEPARATOR)
        {
            p++;
            if (p - value > 80)
                Error("SCRIPT_GetDoubleString: no %c found at end of string %s", SCRIPTSTRINGSEPARATOR, value);
        }

        p++;
        value = p;
        while (*p != SCRIPTSTRINGSEPARATOR)
        {
            p++;
            if (p - value > 80)
                Error("SCRIPT_GetDoubleString: no %c found at end of string %s", SCRIPTSTRINGSEPARATOR, value);
        }
        memcpy(dest2, value, p - value);
        dest2[p - value] = SCRIPTNULL;
    }
}

boolean SCRIPT_GetNumber
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    int32* number
)
{
    char* value;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_GetNumber not a valid scripthandle\n");
    value = SCRIPT_GetRaw(scripthandle, sectionname, entryname);
    if (value)
    {
        if (*value == SCRIPTDEFAULTVALUE)
            return true;
        if (value[0] == SCRIPTHEXFIRST && value[1] == SCRIPTHEXSECOND)
            sscanf(value + 2, "%lx", number);
        else
            sscanf(value, "%ld", number);
    }
    return false;
}

void SCRIPT_GetBoolean
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    boolean* bool
)
{
    int num;
    char* value;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_GetBoolean not a valid scripthandle\n");
    value = SCRIPT_GetRaw(scripthandle, sectionname, entryname);
    if (value)
    {
        sscanf(value, "%ld", &num);
        if (!num)
            *bool = false;
        else
            *bool = true;
    }
}

void SCRIPT_GetDouble
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    double* number
)
{
    char* value;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_GetDouble not a valid scripthandle\n");
    value = SCRIPT_GetRaw(scripthandle, sectionname, entryname);
    if (value)
    {
        if (*value == SCRIPTDEFAULTVALUE)
            return;
        sscanf(value, "%lf", number);
    }
}

void SCRIPT_PutComment(int32 scripthandle, char* sectionname, char* comment)
{
    int len;
    char* p;
    ScriptSectionType* s;
    ScriptLineType* l;
    if (!SCRIPT_HandleValid(scripthandle))
        Error("SCRIPT_PutComment not a valid scripthandle\n");
    s = SCRIPT_SectionExists(scripthandle, sectionname);

    if (!s)
    {
        SCRIPT_AddSection(scripthandle, sectionname);
        s = SCRIPT_SectionExists(scripthandle, sectionname);
    }
    len = strlen(comment) + 1;
    p = SafeMalloc(len);
    strcpy(p, comment);
    l = SCRIPT_AddLine(s->lastline->nextline, linetype_comment, p);
    s->lastline = l;
}

void SCRIPT_PutEOL(int32 scripthandle, char* sectionname)
{
    SCRIPT_PutComment(scripthandle, sectionname, " ");
}

void SCRIPT_PutMultiComment
(
    int32 scripthandle,
    char* sectionname,
    char* comment,
    ...
)
{
    char buf[200];
    va_list va;
    va_start(va, comment);
    vsprintf(buf, comment, va);
    va_end(va);
    SCRIPT_PutComment(scripthandle, sectionname, buf);
}

void SCRIPT_PutSection(int32 scripthandle, char* sectionname)
{
    SCRIPT_AddSection(scripthandle, sectionname);
}

void SCRIPT_PutRaw
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    char* raw
)
{
    SCRIPT_AddEntry(scripthandle, sectionname, entryname, raw);
}

void SCRIPT_PutString
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    char* string
)
{
    char buf[80];
    sprintf(buf, "%c%s%c", SCRIPTSTRINGSEPARATOR, string, SCRIPTSTRINGSEPARATOR);
    SCRIPT_AddEntry(scripthandle, sectionname, entryname, buf);
}

void SCRIPT_PutDoubleString
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    char* string1,
    char* string2
)
{
    char buf[80];
    sprintf(buf, "%c%s%c %c%s%c", SCRIPTSTRINGSEPARATOR, string1, SCRIPTSTRINGSEPARATOR, SCRIPTSTRINGSEPARATOR, string2, SCRIPTSTRINGSEPARATOR);
    SCRIPT_AddEntry(scripthandle, sectionname, entryname, buf);
}

void SCRIPT_PutNumber
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    int32 number,
    boolean hexadecimal,
    boolean defaultvalue
)
{
    char buf[80];
    if (defaultvalue)
        sprintf(buf, "%c", SCRIPTDEFAULTVALUE);
    else if (hexadecimal)
        sprintf(buf, "%c%c%lx", SCRIPTHEXFIRST, SCRIPTHEXSECOND, number);
    else
        sprintf(buf, "%ld", number);

    SCRIPT_AddEntry(scripthandle, sectionname, entryname, buf);
}

void SCRIPT_PutBoolean
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    boolean bool
)
{
    char buf[80];
    if (bool)
        sprintf(buf, "1");
    else
        sprintf(buf, "0");

    SCRIPT_AddEntry(scripthandle, sectionname, entryname, buf);
}

void SCRIPT_PutDouble
(
    int32 scripthandle,
    char* sectionname,
    char* entryname,
    double number,
    boolean defaultvalue
)
{
    char buf[80];
    if (defaultvalue)
        sprintf(buf, "%c", SCRIPTDEFAULTVALUE);
    else
        sprintf(buf, "%f", number);

    SCRIPT_AddEntry(scripthandle, sectionname, entryname, buf);
}
