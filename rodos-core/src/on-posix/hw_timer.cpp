

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
* @author Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief fixed interval timer
*
* class for fixed-interval timer  for Linux as guest os
* uses the virtual timer (counts only when process is executed)
*/

//#include <stdio.h>
//#include <stdlib.h>

#include <signal.h>

#include "hw_specific.h"
#include <sys/time.h>
#include "rodos.h"

/**
* The instruction pointer during the last interruption.
*/
volatile long __interruptedInstructionPointer__ = 0;


namespace RODOS {
extern "C" void exit(int c);

extern volatile long taskRunning;

/**
* Size of stack required by signal handler.
*/
#define SIGNAL_HANDLER_STACK_SIZE SIGSTKSZ

/** the timer interval */
long long Timer::microsecondsInterval = 0;



/**
* the signal handler for SIGVTALRM (timer signal)
*/
void timerSignalHandler(int ignore) {
  
    // time events to call?
    TTime timeNow = NOW();  // comment this out to improve performance, but: no time events any more
    TimeEvent::propagate(timeNow); // comment this out to improve performance, but: no time events any more
}

/**
* initialize the timer and signal handler 
*/
void Timer::init() {
//Signal handlers are called synchronously from the main thread, see scheduler.cpp
	 //signal(SIGALRM, timerSignalHandler);
}

/**
* start timer 
*/
void Timer::start() {
  itimerval params;
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
