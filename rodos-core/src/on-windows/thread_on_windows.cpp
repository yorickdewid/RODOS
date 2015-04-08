

/*********************************************************** Copyright
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/


/**
* @file thread.cc
* @date 2010/09/22 16:50
* @author Sergio Montenegro
*
* Copyright 2008 DLR
*
* A Thread is a schedulable object with own context and stack
*
* @brief %Thread handling
*/

#include "hw_specific.h"
#include "thread.h"
#include "application.h"
#include "debug.h"
#include <windows.h>
#include <stdio.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

// Diese defines werden nur bei windows inter benutz, daher sollen
// sie in src/on_windows definiert sein und nur dort sichtbar sein
// sonst haben wir problemen mit den anderen 12 implementierungen

#define CREATE_WIN_THREAD_PRIORITY(PRI_CLASS,PRI)  (((long) (PRI_CLASS & 0x0F) << 4) | (long) (PRI & 0x0F))

//Thread Priorities
#define WIN_THREAD_PRIORITY_IDLE           0x00
#define WIN_THREAD_PRIORITY_LOW            0x01
#define WIN_THREAD_PRIORITY_NORMAL         0x02
#define WIN_THREAD_PRIORITY_HIGH           0x03
#define WIN_THREAD_PRIORITY_TIMECRITICAL   0x04
#define WIN_THREAD_PRIORITY_REALTIME       0x05


typedef struct
{ HANDLE         hThread;
  unsigned long  lThreadId;
  unsigned long  ulPriorityClass;
  unsigned long  ulPriority;
} t_Thread_OnWindows;

// ***************************************************************************

/** constructor */
Thread::Thread(const char* name, const long  priority1, const long _stackSize)
: ListElement(threadList, name) 
{ t_Thread_OnWindows *pcThread = new t_Thread_OnWindows;

  stackSize        = _stackSize;  // not used in windows
  priority         = priority1;
  lastActivation   = 0;		       // not used in windows

  pcThread->ulPriorityClass = (priority >> 4) & 0x0F;
  pcThread->ulPriority      = (priority >> 0) & 0x0F;
  pcThread->hThread         = 0;
  pcThread->lThreadId       = 0;

  suspendedUntil = 0;		  // Ready to run
  waitingFor     = 0;		  // not waiting for any one
  context        = (volatile long *) pcThread;
  
  nextBeat = END_OF_TIME;        // no period defined
  period   = 1;

}

// ***************************************************************************

Thread::~Thread() 
{ 
  delete context;

  PRINTF("%s:",getName());
  PRINTF("Thread deleted");
}

// ***************************************************************************

//void Thread::put(const long topicId, const int msgLen, const void* msg) {}

// ***************************************************************************

void Thread::initializeThreads() {
    PRINTF("Threads in System:");
    ITERATE_LIST(Thread, threadList) {
        PRINTF("\n   Prio = %7ld Stack = %6ld %s: ", iter->priority, iter->stackSize, iter->getName());
        iter->init();
        iter->suspendedUntil = 0;
    }
    PRINTF("\n");
}

// ***************************************************************************

Thread* Thread::getCurrentThread() 
{ t_Thread_OnWindows *pcThread;

  DWORD   hThreadID = GetCurrentThreadId();
  Thread *pcThis  = 0;

  ITERATE_LIST(Thread, threadList) 
  { pcThread = (t_Thread_OnWindows *) iter->context;
    if(pcThread->lThreadId == hThreadID) 
      return iter;
  }
//PRINTF("Windows theads-id %ld not ready? !\n", hThreadID);

  return 0;
}

// ***************************************************************************

static bool threadsGO = false; // to wait until all threads are ready
static unsigned long __stdcall WindowsThreadEntryPoint (void *param)
{ Thread *pcThread = (Thread *) param;

  if (pcThread == 0) return 1;

  while(!threadsGO) Sleep(5);

  pcThread->run();

  return 0;
}

// ***************************************************************************

/* called in main() after all constuctors, to create/init thread */
void Thread::create() 
{ t_Thread_OnWindows *pcThread = (t_Thread_OnWindows *) context;

  if (pcThread->hThread || pcThread->lThreadId) return;

  //Thread erstellen und starten
  pcThread->hThread = CreateThread (0, 0 /* stackSize */, &WindowsThreadEntryPoint, this, 0, &(pcThread->lThreadId));
  PRINTF("Created windows theads-id: %ld\n", pcThread->lThreadId);

}

// ***************************************************************************

void Thread::startAllThreads() 
{
  ITERATE_LIST(Thread, threadList) 
    iter->create();

  threadsGO = true;
  PRINTF("--------------- system run now ----------------\n");
}

// ***************************************************************************

/** pause execution of this thread and call scheduler */
void Thread::yield() {
	Sleep(0);
}

// ***************************************************************************

/* resume the thread */
void Thread::resume() 
{ t_Thread_OnWindows *pcThread = (t_Thread_OnWindows *) context;

  if (!pcThread->hThread) return;

  suspendedUntil = 0;
  waitingFor = 0;

  ResumeThread (pcThread->hThread); 
}

// ***************************************************************************

unsigned long long Thread::getScheduleCounter() { return 0; }

// ***************************************************************************
// ***************************************************************************
// ***************************************************************************

/* get priority of the thread */
long Thread::getPriority() const 
{ t_Thread_OnWindows *pcThread = (t_Thread_OnWindows *) context;
  long lPrioClass, lPrio;

  lPrioClass = GetPriorityClass (pcThread->hThread);
  lPrio      = GetThreadPriority(pcThread->hThread);
  
  //Übersetzung in OIS Params
  switch (lPrioClass)
  { case IDLE_PRIORITY_CLASS:     pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_IDLE;          break;
    case NORMAL_PRIORITY_CLASS:   pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_NORMAL;        break;
    case HIGH_PRIORITY_CLASS:     pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_TIMECRITICAL;  break;
    case REALTIME_PRIORITY_CLASS: pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_REALTIME;      break;
    default:                      pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_NORMAL;
  }

  switch (lPrio)
  { case THREAD_PRIORITY_IDLE:          pcThread->ulPriority = WIN_THREAD_PRIORITY_IDLE;          break;
    case THREAD_PRIORITY_BELOW_NORMAL:  pcThread->ulPriority = WIN_THREAD_PRIORITY_LOW;           break;
    case THREAD_PRIORITY_NORMAL:        pcThread->ulPriority = WIN_THREAD_PRIORITY_NORMAL;        break;
    case THREAD_PRIORITY_ABOVE_NORMAL:  pcThread->ulPriority = WIN_THREAD_PRIORITY_HIGH;          break;
    case THREAD_PRIORITY_TIME_CRITICAL: pcThread->ulPriority = WIN_THREAD_PRIORITY_TIMECRITICAL;  break;
    default:                            pcThread->ulPriority = WIN_THREAD_PRIORITY_NORMAL;
  }
  
  return ((pcThread->ulPriorityClass & 0x0F) << 4) | (pcThread->ulPriority & 0x0F);
}

// ***************************************************************************

/* set priority of the thread */
void Thread::setPriority (const long prio)
{ t_Thread_OnWindows *pcThread = (t_Thread_OnWindows *) context;
  DWORD  dw;

  pcThread->ulPriorityClass = (prio >> 4) & 0x0F;
  pcThread->ulPriority      = (prio >> 0) & 0x0F;
  
  //is the thread running?
  if (!pcThread->hThread) return;
  
  switch (pcThread->ulPriorityClass)
  { case WIN_THREAD_PRIORITY_IDLE:          pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_IDLE;         dw = IDLE_PRIORITY_CLASS;      break;
    case WIN_THREAD_PRIORITY_LOW:           pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_LOW;          dw = NORMAL_PRIORITY_CLASS;    break;
    case WIN_THREAD_PRIORITY_NORMAL:        pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_NORMAL;       dw = NORMAL_PRIORITY_CLASS;    break;
    case WIN_THREAD_PRIORITY_HIGH:          pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_HIGH;         dw = NORMAL_PRIORITY_CLASS;    break;
    case WIN_THREAD_PRIORITY_TIMECRITICAL:  pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_TIMECRITICAL; dw = HIGH_PRIORITY_CLASS;      break;
    case WIN_THREAD_PRIORITY_REALTIME:      pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_REALTIME;
                                            pcThread->ulPriority      = WIN_THREAD_PRIORITY_NORMAL;       dw = REALTIME_PRIORITY_CLASS;  break;
    default:                                pcThread->ulPriorityClass = WIN_THREAD_PRIORITY_NORMAL;       dw = NORMAL_PRIORITY_CLASS;
  }
  SetPriorityClass(GetCurrentProcess(), dw);

  switch (pcThread->ulPriority)
  { case WIN_THREAD_PRIORITY_IDLE:         pcThread->ulPriority=WIN_THREAD_PRIORITY_IDLE;         dw = THREAD_PRIORITY_IDLE;          break;
    case WIN_THREAD_PRIORITY_LOW:          pcThread->ulPriority=WIN_THREAD_PRIORITY_LOW;          dw = THREAD_PRIORITY_BELOW_NORMAL;  break;
    case WIN_THREAD_PRIORITY_NORMAL:       pcThread->ulPriority=WIN_THREAD_PRIORITY_NORMAL;       dw = THREAD_PRIORITY_NORMAL;        break;
    case WIN_THREAD_PRIORITY_HIGH:         pcThread->ulPriority=WIN_THREAD_PRIORITY_HIGH;         dw = THREAD_PRIORITY_ABOVE_NORMAL;  break;
    case WIN_THREAD_PRIORITY_TIMECRITICAL: pcThread->ulPriority=WIN_THREAD_PRIORITY_TIMECRITICAL; dw = THREAD_PRIORITY_TIME_CRITICAL; break;
    default:                               pcThread->ulPriority=WIN_THREAD_PRIORITY_NORMAL;       dw = THREAD_PRIORITY_NORMAL;        break;
  }
  SetThreadPriority(pcThread->hThread, dw);
}

// ***************************************************************************
// ***************************************************************************
// ***************************************************************************

extern long long timeAtStartup; 

/* suspend the thread */
bool Thread::suspendCallerUntil(const TTime reactivationTime, void *signaler) 
{ Thread       *caller   = getCurrentThread();
  long long     now      = NOW();
  long long     timeDiff = reactivationTime - now;
  unsigned long timeToWait;

  if (caller == 0)
  { if (reactivationTime > now)
    { timeToWait = (long)(timeDiff / 1000000);  // to wait time im milliseconds
      Sleep (timeToWait);
    }
    return false;
  }

  caller->waitingFor     = signaler;
  caller->suspendedUntil = reactivationTime;

  if (caller->suspendedUntil > now) 
  { timeToWait = (long)(timeDiff / 1000000);  // to wait time im milliseconds
    Sleep (timeToWait);
  }

  /** after yield: It was resumed (suspendedUntil set to 0) or time was reached ?*/
  if(caller->suspendedUntil == 0) return true; // it was resumed!
  return false; // time was reached
}

// ***************************************************************************

//void Thread::suspendUntilNextBeat ()
//{ long long llNow  =  NOW();
//  long long llRest = (llNow - nextBeat) % period;
//
//  if (nextBeat > llNow)
//    suspendCallerUntil (nextBeat);
//  else
//  { nextBeat = (llNow - llRest) + period;
//    suspendCallerUntil (nextBeat);
//  }
//}

// ***************************************************************************

//void Thread::setPeriodicBeat (const TTime begin, const TTime period1)
//{
//  nextBeat = timeAtStartup + begin;    ///<  the next time to awake (used in wait)
//  period   = period1;                  ///<  To repeate every period localTime units
//}

/****************************************************************************/

// not used in posix: posix has its own scheluder
//Thread* Thread::findNextToRun(TTime timeNow) { }

Thread* Thread::findNextWaitingFor(void* signaler) 
{ Thread* nextWaiter = 0;

  ITERATE_LIST(Thread, threadList) 
  { if (iter->waitingFor == signaler) 
      if (nextWaiter == 0 || (iter->getPriority() > nextWaiter->getPriority())) 
        nextWaiter = iter;
  }
  return nextWaiter;
}

// ***************************************************************************

int32_t Thread::getMaxStackUsage(){ return 0; }


#ifndef NO_RODOS_NAMESPACE
}
#endif

