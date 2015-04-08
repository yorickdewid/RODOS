

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
* @file hw_specific.cc
* @date 2010/09/23 7:33
* @author Lutz Dittrich, Sergio Montenegro
*
* Copyright 2008 DLR
*
* @brief all hardware specific stuff I have no better place for ...
*
*/

#include "hw_specific.h"
#include "rodos.h"

// #include <stdlib.h> 
#include <stdio.h> 
#include <signal.h>
#include <unistd.h>

namespace RODOS {
extern "C" void abort();

volatile long taskRunning = 0;

/**
* initialize hardware specific stuff
*/

extern void sigtermHandler(int sig);
extern long myNodeNr;

void hwInit() {
  taskRunning = 0;

  /* External interrupts simulator */
  signal(SIGTERM, sigtermHandler);

 /** best effort to determine the node noumber (not perfect or even ok!).
     This number may be changed by the gateway link interface
     according to other chriteria **/
  // myNodeNr = (getpid() % (MAX_NUMBER_OF_NODES -1) ) + 1; ORIGINAL
  myNodeNr = getpid(); // a default node number, may be replaced by the link interface (gateway)
}

/**
*  abort execution
*/
void hwResetAndReboot() {
  xprintf("hw_resetAndReboot() -> exit");
  abort();
}

/**
  *  global interrupt disable
  */

/* --> iplemented in scheduler.cpp
void hwDisableInterrupts(){
    #warning "no interrupt disable implemented"

}

void hwEnableInterrupts(){
    #warning "no interrupt enable implemented"
}

*/

static long long timeOfLastTriggerWatchdog = 0;
void hwInitWatchdog(long intervalMilliseconds) { }
void hwTriggerWatchdog() { timeOfLastTriggerWatchdog = NOW(); }


/**
* create context on stack and return a pointer to it
*/
long* hwInitContext(long* stack, void* object) {
  return (long*) 0;
}


/******************************************/
void sp_partition_yield() {}

void FFLUSH() { fflush(stdout); }

} // namespace RODOS
