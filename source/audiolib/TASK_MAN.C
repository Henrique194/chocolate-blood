/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/**********************************************************************
   module: TASK_MAN.C

   author: James R. Dose
   date:   July 25, 1994

   Low level timer task scheduler.

   (c) Copyright 1994 James R. Dose.  All Rights Reserved.
**********************************************************************/

#define TRUE  ( 1 == 1 )
#define FALSE ( !TRUE )

#define USE_USRHOOKS

#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>
#include "compat.h"
#include "system.h"
#include "sound.h"
#include "linklist.h"
#include "task_man.h"

#ifdef USE_USRHOOKS
#include "usrhooks.h"
#define FreeMem( ptr )   USRHOOKS_FreeMem( ( ptr ) )
#else
#define FreeMem( ptr )   free( ( ptr ) )
#endif

typedef struct
   {
   task *start;
   task *end;
   } tasklist;


/*---------------------------------------------------------------------
   Global variables
---------------------------------------------------------------------*/

static task HeadTask;
static task *TaskList = &HeadTask;

static volatile int32_t TaskServiceRate  = 0x10000L;

static char TS_Installed = FALSE;

volatile int TS_InInterrupt = FALSE;

static SDL_Mutex *task_mutex;
int TS_InterruptDisabled;

/*---------------------------------------------------------------------
   Function prototypes
---------------------------------------------------------------------*/

static void TS_FreeTaskList( void );
static void TS_SetClockSpeed( int32_t speed );
static int32_t TS_SetTimer( int32_t TickBase );
static void TS_SetTimerToMaxTaskRate( void );
static void TS_ServiceScheduleTimer( void );
static void TS_ServiceScheduleMusic( void );
static void TS_AddTask( task *ptr );
static int  TS_Startup( void );
static void RestoreRealTimeClock( void );


/*---------------------------------------------------------------------
   Function: TS_FreeTaskList

   Terminates all tasks and releases any memory used for control
   structures.
---------------------------------------------------------------------*/

static void TS_FreeTaskList
   (
   void
   )

   {
   task *node;
   task *next;
  
   SDL_LockMutex(task_mutex);

   node = TaskList->next;
   while( node != TaskList )
      {
      next = node->next;
      FreeMem( node );
      node = next;
      }

   TaskList->next = TaskList;
   TaskList->prev = TaskList;

   SDL_UnlockMutex(task_mutex);
   }


/*---------------------------------------------------------------------
   Function: TS_SetClockSpeed

   Sets the rate of the 8253 timer.
---------------------------------------------------------------------*/

static void TS_SetClockSpeed
   (
   int32_t speed
   )

   {
   SDL_LockMutex(task_mutex);

   if ( ( speed > 0 ) && ( speed < 0x10000L ) )
      {
      TaskServiceRate = speed;
      }
   else
      {
      TaskServiceRate = 0x10000L;
      }

   Sys_SetTimer(TaskServiceRate, TS_ServiceScheduleTimer);
   Music_SetTimer(TaskServiceRate, TS_ServiceScheduleMusic);

   SDL_UnlockMutex(task_mutex);
   }


/*---------------------------------------------------------------------
   Function: TS_SetTimer

   Calculates the rate at which a task will occur and sets the clock
   speed if necessary.
---------------------------------------------------------------------*/

static int32_t TS_SetTimer
   (
   int32_t TickBase
   )

   {
   int32_t speed;

   speed = 1192030L / TickBase;
   if ( speed < TaskServiceRate )
      {
      TS_SetClockSpeed( speed );
      }

   return( speed );
   }


/*---------------------------------------------------------------------
   Function: TS_SetTimerToMaxTaskRate

   Finds the fastest running task and sets the clock to operate at
   that speed.
---------------------------------------------------------------------*/

static void TS_SetTimerToMaxTaskRate
   (
   void
   )

   {
   task     *ptr;
   int32_t      MaxServiceRate;

   MaxServiceRate = 0x10000L;

   ptr = TaskList->next;
   while( ptr != TaskList )
      {
      if ( ptr->rate < MaxServiceRate )
         {
         MaxServiceRate = ptr->rate;
         }

      ptr = ptr->next;
      }

   if ( TaskServiceRate != MaxServiceRate )
      {
      TS_SetClockSpeed( MaxServiceRate );
      }
   }

/*---------------------------------------------------------------------
   Function: TS_ServiceSchedule

   Interrupt service routine
---------------------------------------------------------------------*/

static void TS_ServiceSchedule
   (
   int channel
   )

   {
   task *ptr;
   task *next;

   if (TS_InterruptDisabled)
       return;

   SDL_LockMutex(task_mutex);

   TS_InInterrupt = TRUE;

   ptr = TaskList->next;
   while( ptr != TaskList )
      {
      next = ptr->next;

      if ( ptr->active && ptr->channel == channel )
         {
         ptr->count += TaskServiceRate;
//JIM
//         if ( ptr->count >= ptr->rate )
         while( ptr->count >= ptr->rate )
            {
            ptr->count -= ptr->rate;
            ptr->TaskService( ptr );
            }
         }
      ptr = next;
      }

   TS_InInterrupt = FALSE;
   SDL_UnlockMutex(task_mutex);
   }

static void TS_ServiceScheduleTimer
   (
   void
   )
   {
   TS_ServiceSchedule(TASK_CHANNEL_TIMER);
   }

static void TS_ServiceScheduleMusic
   (
   void
   )
   {
   TS_ServiceSchedule(TASK_CHANNEL_MUSIC);
   }

/*---------------------------------------------------------------------
   Function: TS_Startup

   Sets up the task service routine.
---------------------------------------------------------------------*/

static int TS_Startup
   (
   void
   )

   {
   if ( !TS_Installed )
      {
//static const task *TaskList = &HeadTask;
      TaskList->next = TaskList;
      TaskList->prev = TaskList;

      TaskServiceRate  = 0x10000L;

      task_mutex = SDL_CreateMutex();

      Sys_SetTimer(TaskServiceRate, TS_ServiceScheduleTimer);
      Music_SetTimer(TaskServiceRate, TS_ServiceScheduleMusic);

      TS_Installed = TRUE;
      }

   return( TASK_Ok );
   }


/*---------------------------------------------------------------------
   Function: TS_Shutdown

   Ends processing of all tasks.
---------------------------------------------------------------------*/

void TS_Shutdown
   (
   void
   )

   {
   if ( TS_Installed )
      {
      TS_FreeTaskList();

      TS_SetClockSpeed( 0 );

      Sys_SetTimer(0, NULL);
      Music_SetTimer(0, NULL);

      TS_Installed = FALSE;
      }
   }


/*---------------------------------------------------------------------
   Function: TS_ScheduleTask

   Schedules a new task for processing.
---------------------------------------------------------------------*/

task *TS_ScheduleTask
   (
   void  ( *Function )( task * ),
   int   rate,
   int   priority,
   void *data,
   int channel
   )

   {
   task *ptr;

#ifdef USE_USRHOOKS
   int   status;

   ptr = NULL;

   status = USRHOOKS_GetMem( &ptr, sizeof( task ) );
   if ( status == USRHOOKS_Ok )
#else
   ptr = malloc( sizeof( task ) );
   if ( ptr != NULL )
#endif
      {
      if ( !TS_Installed )
         {
         status = TS_Startup();
         if ( status != TASK_Ok )
            {
            FreeMem( ptr );
            return( NULL );
            }
         }

      SDL_LockMutex(task_mutex);

      ptr->TaskService = Function;
      ptr->data = data;
      ptr->rate = TS_SetTimer( rate );
      ptr->count = 0;
      ptr->priority = priority;
      ptr->channel = channel;
      ptr->active = FALSE;

      TS_AddTask( ptr );

      SDL_UnlockMutex(task_mutex);
      }

   return( ptr );
   }


/*---------------------------------------------------------------------
   Function: TS_AddTask

   Adds a new task to our list of tasks.
---------------------------------------------------------------------*/

static void TS_AddTask
   (
   task *node
   )

   {
   LL_SortedInsertion( TaskList, node, next, prev, task, priority );
   }


/*---------------------------------------------------------------------
   Function: TS_Terminate

   Ends processing of a specific task.
---------------------------------------------------------------------*/

int TS_Terminate
   (
   task *NodeToRemove
   )

   {
   task *ptr;
   task *next;

   SDL_LockMutex(task_mutex);

   ptr = TaskList->next;
   while( ptr != TaskList )
      {
      next = ptr->next;

      if ( ptr == NodeToRemove )
         {
         LL_RemoveNode( NodeToRemove, next, prev );
         NodeToRemove->next = NULL;
         NodeToRemove->prev = NULL;
         FreeMem( NodeToRemove );

         TS_SetTimerToMaxTaskRate();

         SDL_UnlockMutex(task_mutex);

         return( TASK_Ok );
         }

      ptr = next;
      }

   SDL_UnlockMutex(task_mutex);

   return( TASK_Warning );
   }


/*---------------------------------------------------------------------
   Function: TS_Dispatch

   Begins processing of all inactive tasks.
---------------------------------------------------------------------*/

void TS_Dispatch
   (
   void
   )

   {
   task *ptr;

   SDL_LockMutex(task_mutex);

   ptr = TaskList->next;
   while( ptr != TaskList )
      {
      ptr->active = TRUE;
      ptr = ptr->next;
      }

   SDL_UnlockMutex(task_mutex);
   }


/*---------------------------------------------------------------------
   Function: TS_SetTaskRate

   Sets the rate at which the specified task is serviced.
---------------------------------------------------------------------*/

void TS_SetTaskRate
   (
   task *Task,
   int rate
   )

   {
   SDL_LockMutex(task_mutex);

   Task->rate = TS_SetTimer( rate );
   TS_SetTimerToMaxTaskRate();

   SDL_UnlockMutex(task_mutex);
   }
