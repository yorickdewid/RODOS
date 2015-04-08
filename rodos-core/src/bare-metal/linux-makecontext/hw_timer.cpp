

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
* @file hw_timer.cc
* @date 2008/04/23 16:36
*
* @brief fixed interval timer
*
* class for fixed-interval timer  for Linux as guest os
* uses the virtual timer (counts only when process is executed)
*/

// #include <stdio.h>
// #include <stdlib.h>

#include <signal.h>

#include "hw_specific.h"
#include "rodos.h"
#include <sys/time.h>

#include <ucontext.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/**
* The instruction pointer during the last interruption.
*/
volatile long __interruptedInstructionPointer__ = 0;



namespace RODOS {

extern "C" void exit(int c);

/**
* Size of stack required by signal handler.
*/
#define SIGNAL_HANDLER_STACK_SIZE SIGSTKSZ

/** the timer interval */
long long Timer::microsecondsInterval = 0;



extern "C" {
  /**
  * Declaration of function with C linkage.
  * Is implemented in assembly.
  * Saves the current context and calls the scheduler.
  */
  extern void __asmSaveContext();
}

extern long long timeToTryAgainToSchedule;
extern bool isSchedulingEnabled;
extern void *signal_stack;

/**
* the signal handler for SIGVTALRM (timer signal)
*/
void timerSignalHandler(int j, siginfo_t *si, void *old_context) {

   if(!isSchedulingEnabled) return;
   long long timeNow = NOW();  // comment this out to improve performance, but: no time events any more
   TimeEvent::propagate(timeNow); // comment this out to improve performance, but: no time events any more

   if(timeNow < timeToTryAgainToSchedule) return;
   __asmSaveContextAndCallScheduler();
}

/**
* initialize the timer and signal handler 
*/
void Timer::init() {
  static int once = 0;
  if (once == 0) {
    int retval;
    struct sigaction action;

    once = 1;
    signal_stack = xmalloc(4096);
    /* callback function for signal handling */
    action.sa_sigaction = timerSignalHandler;

    /* SA_ONSTACK - signal handler should use its own stack */ //<- disabled for the use of makecontext with newlib
    /* SA_RESTART - interrupted system calls shall be restartet */
    action.sa_flags = SA_SIGINFO | SA_RESTART;

    /* empty signal set */
    sigemptyset(&action.sa_mask);

    retval = sigaction(SIGALRM,&action,0);
    if (retval != 0) {
      xprintf("error during call to sigaction\n");
      exit(1);
    }
  }
}

/**
* start timer 
*/
void Timer::start() {
  struct itimerval params;
  int retval;
  params.it_interval.tv_sec  = microsecondsInterval / 1000000;
  params.it_interval.tv_usec = microsecondsInterval % 1000000;
  params.it_value.tv_sec     = microsecondsInterval / 1000000;
  params.it_value.tv_usec    = microsecondsInterval % 1000000;
  retval = setitimer(ITIMER_REAL,&params,0);
  if (retval != 0) {
    xprintf("error during call to setitimer\n");
    exit(1);
  }

}

/**
* stop timer 
*/
void Timer::stop() {
  struct itimerval params;
  int retval;
  params.it_interval.tv_sec = 0;
  params.it_interval.tv_usec = 0;
  params.it_value.tv_sec = 0;
  params.it_value.tv_usec = 0;
  retval = setitimer(ITIMER_REAL,&params,0);
  if (retval != 0) {
    xprintf("error during call to setitimer\n");
    exit(1);
  }
}

/**
* set timer interval 
*/
void Timer::setInterval(const long long microsecondsInterval) {
  Timer::microsecondsInterval = microsecondsInterval;
}
}
