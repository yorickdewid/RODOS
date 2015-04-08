

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
* @file scheduler.cc
* @date 2008/04/24 18:10
* @author Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief priority based scheduler
*
*/
#include "rodos.h"
#include "scheduler.h"
#include "hw_specific.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

extern "C" {
  /**
  * A piece of memory used as stack by the scheduler.
  */
  char __schedulerStack__[SCHEDULER_STACKSIZE];

  /**
  * Address of the top of the scheduler's stack.
  * 8-byte aligned
  */
  long __schedulerStackFrame__ =
                  ((long)&(__schedulerStack__[SCHEDULER_STACKSIZE-sizeof(long)])) & (~7);
}

/** count all calls to the scheduler */
unsigned long long Scheduler::scheduleCounter=0;
Thread* Scheduler::preSelectedNextToRun = 0;
long long Scheduler::preSelectedTime = 0;

bool isSchedulingEnabled = true; ///< will be checkt before some one calls scheduler::schedule

void globalAtomarLock()   { isSchedulingEnabled = false; }
void globalAtomarUnlock() { isSchedulingEnabled = true;  }


void schedulerWrapper(long* ctx) {
  Thread::currentThread->context=ctx;
  Scheduler::schedule();
}

extern Thread* idlethreadP;

/** activate idle thread */
void Scheduler::idle() {
  idlethreadP->suspendedUntil = 0;


  Thread::currentThread = idlethreadP;
  taskRunning = 1;  /* a bit to early, but no later place possible */

  /* - the order of activate() and startIdleThread() is important -> don't change
   * - For all cortex ports a global context pointer is initialized in activate()
   *   and this must have been done before startIdleThread() is called.
   */
  idlethreadP->activate();

  startIdleThread(); // only for some architectures, most implementations == nop()
  
}

void Scheduler::schedule() {
  Scheduler::scheduleCounter++;

  /** Optimisations: if Thread::yeild() prepared time and next to run, unse it, but only once! **/
  TTime timeNow = preSelectedTime;  // eventaully set by Thread::yield() 
  if(timeNow == 0) timeNow = NOW(); // ovoisly not set, then recompute
  preSelectedTime = 0;              // use only onece

  // time events to call?
  // now obsolet! call direcly from timer!! TimeEvent::propagate(timeNow);

  /** select the next thread to run: Do we have a preselection from Thread::yield()? **/
  Thread* nextThreadToRun = preSelectedNextToRun; // eventually set by Thread::yeild()
  if(nextThreadToRun == 0)  nextThreadToRun = Thread::findNextToRun(timeNow); // not the case
  preSelectedNextToRun = 0;                      // use ony once

  // now activate the selected trhead
  nextThreadToRun->lastActivation = Scheduler::scheduleCounter; // timeNow ?? but waht with on-os_xx, on-posix, etc?
  nextThreadToRun->activate();
}

unsigned long long Scheduler::getScheduleCounter() {
  return scheduleCounter;
}


/** returns true if scheduler is considered to be running */
bool Scheduler::isSchedulerRunning() {
  return (taskRunning > 0);
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

