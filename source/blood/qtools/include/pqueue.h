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
#ifndef _PQUEUE_H_
#define _PQUEUE_H_
#include "typedefs.h"

#define kPQueueSize 1024

class PriorityQueue
{
public:
    PriorityQueue();
    void Upheap(void);
    void Downheap(uint);
    void Delete(uint);
    void Insert(uint32_t, uint32_t);
    uint32_t Remove(void);

    struct queueItem
    {
        uint32_t at0; // priority
        uint32_t at4; // data
    } queueItems[kPQueueSize + 1];

    int fNodeCount; // at2008
};

#endif
