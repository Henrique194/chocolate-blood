/*
 * Copyright (C) 2018, 2022 nukeykt
 *
 * This file is part of Blood-RE.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _INIFILE_H_
#define _INIFILE_H_

#include "typedefs.h"


struct IniNode {
    IniNode *next;
    char f_4[1];
};

class IniFile
{
public:
    IniFile(const char *);
    IniFile(void *, int dummy);
    void Load(void *);
    void Load(void);
    void Save(void);
    QBOOL FindSection(const char *);
    QBOOL FindKey(const char*);
    void AddSection(const char *);
    void AddKeyString(const char *, const char *);
    void ChangeKeyString(const char *, const char *);
    QBOOL SectionExists(const char *);
    QBOOL KeyExists(const char *, const char *);
    void PutKeyString(const char *, const char *, const char *);
    const char *GetKeyString(const char *, const char *, const char *);
    void PutKeyInt(const char *, const char *, int);
    int GetKeyInt(const char *, const char *, int);
    QBOOL GetKeyBool(const char *, const char *, int);
    void PutKeyHex(const char *, const char *, int);
    int GetKeyHex(const char *, const char *, int);
    void RemoveKey(const char *, const char *);
    void RemoveSection(const char *);
    ~IniFile(void);

    IniNode f_0;
    IniNode *curNode;
    IniNode *f_9;

    char *f_d;

    char f_11[_MAX_PATH];

};


#endif
