

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

#include <xmk.h>
#include <sys/init.h>
#include <platform.h>

#include <stdlib.h>
#include <signal.h>
#include <sys/process.h>

#include "hw_specific.h"
#include "rodos.h"
#include "scheduler.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

volatile long taskRunning = 0;

/**
* initialize hardware specific stuff
*/

extern void sigtermHandler(int sig);

void hwInit() {

  init_platform();

  xilkernel_init();
  //PRINTF ("xilkernel init done\n");
  taskRunning = 0;

  /* External interrupts simulator */
  signal(SIGTERM, sigtermHandler);
}

/**
*  abort execution
*/
void hwResetAndReboot() {
  PRINTF("hw_resetAndReboot() -> exit");
  abort();
}

/**
  *  global interrupt disable
  */
void hwDisableInterrupts(){
    #warning "no interrupt disable implemented"
}

/**
  *  global interrupt enable
  */
void hwEnableInterrupts(){
    #warning "no interrupt enable implemented"
}

static long long timeOfLastTriggerWatchdog = 0;
void hwInitWatchdog(long intervalMilliseconds) { }
void hwTriggerWatchdog() { timeOfLastTriggerWatchdog = NOW(); }


/**
* create context on stack and return a pointer to it
*/
long* hwInitContext(long* stack, void* object) {
  return (long*) 0;
}

/**
 * In case a network is avialable, the corresponding node nr.
 */

long myNodeNr = -1 ;

long getNodeNumber() { return myNodeNr; }

/******************************************/
//extern int yield(void);
void sp_partition_yield() { yield(); }


#ifndef NO_RODOS_NAMESPACE
}
#endif

