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
#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "typedefs.h"
#include "qheap.h"

#pragma pack(push, 1)

struct RFFHeader
{
    char sign[4];
    short version;
    short pad1;
    uint32_t offset;
    uint32_t filenum;
    int pad2[4];
};

struct CACHENODE
{
    QPtr<void> ptr;
    QPtr<CACHENODE> prev;
    QPtr<CACHENODE> next;
    int lockCount;
};

struct DICTNODE : CACHENODE
{
    uint32_t offset;
    uint32_t size;
    int pad1[2];
    byte flags;
    char type[3];
    char name[8];
    int id;
};

#pragma pack(pop)

enum {
    kResourceFlag1 = 1,
    kResourceFlag2 = 2,
    kResourceFlag3 = 4,
    kResourceFlag4 = 8,
    kResourceFlag5 = 16,
};

class Resource
{
public:
    Resource();
    ~Resource();

    void Init(const char *filename, const char *external);
    static void Flush(CACHENODE *h);
    void Purge(void);
    DICTNODE **Probe(const char *fname, const char *type);
    DICTNODE **Probe(uint32_t id, const char *type);
    void Reindex(void);
    void Grow(void);
    void AddExternalResource(const char *name, const char *type, int size);
    static void *Alloc(int32_t nSize);
    static void Free(void *p);
    DICTNODE *Lookup(const char *name, const char *type);
    DICTNODE *Lookup(uint32_t id, const char *type);
    void Read(DICTNODE *n);
    void Read(DICTNODE *n, void *p);
    void *Load(DICTNODE *h);
    void *Load(DICTNODE *h, void *p);
    void *Lock(DICTNODE *h);
    void Unlock(DICTNODE *h);
    void Crypt(byte *p, int32_t length, int key);
    static void AddMRU(CACHENODE* h)
    {
        h->prev = purgeHead->prev;
        ((CACHENODE*)h->prev)->next = h;
        h->next = purgeHead;
        ((CACHENODE*)h->next)->prev = h;
    }
    static void RemoveMRU(CACHENODE* h)
    {
        ((CACHENODE*)h->prev)->next = h->next;
        ((CACHENODE*)h->next)->prev = h->prev;
    }
    static int Size(DICTNODE *n) { return n->size; }

    DICTNODE *dict;
    DICTNODE **indexName;
    DICTNODE **indexId;
    int buffSize;
    int count;
    int handle;
    QBOOL crypt;
    QBOOL f_19;
    char ext[144];

    static QHeap *heap;
    static CACHENODE *purgeHead;
};

#endif
