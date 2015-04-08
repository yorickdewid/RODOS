

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
* @date 2010/09/24 18:10
* @author Sergio Montenegro
*
* Copyright 2008 DLR
*
* @brief priority based scheduler
*
*/

#include <xmk.h>
#include <sys/init.h>
#include <platform.h>

#include <unistd.h>
#include <pthread.h>

#include "rodos.h"
#include "scheduler.h"
#include "hw_specific.h"




#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/** count all calls to the scheduler */
unsigned long long Scheduler::scheduleCounter=0;


void schedulerWrapper(long* ctx) {
}

extern "C" void *init_stuff (void *)
{
    // PRINTF ("Starting all Threads...\n");
    Thread::startAllThreads();
    // PRINTF ("Starting Timer...\n");
    Timer::start();
    return 0;
}

/** activate idle thread */
void Scheduler::idle() {
    xmk_add_static_thread(init_stuff, 0);
    xilkernel_start ();
}

// Not used in posix, becouse posix hast its own schedulder
// void Scheduler::schedule() { }

unsigned long long Scheduler::getScheduleCounter() {
  return scheduleCounter;
}


/** returns true if scheduler is considered to be running */
bool Scheduler::isSchedulerRunning() {
 return (taskRunning >  0);
}



#ifndef NO_RODOS_NAMESPACE
}
#endif

