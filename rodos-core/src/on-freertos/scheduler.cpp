

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
*
* @brief priority based scheduler
*
*/
#include "rodos.h"
#include "scheduler.h"
#include "hw_specific.h"

// FreeRTOS includes
#include "FreeRTOS.h"  // Contains type specifications of FreeRTOS
#include "task.h"      // Tasks and Scheduler


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/** count all calls to the scheduler */
unsigned long long Scheduler::scheduleCounter=0;

void schedulerWrapper(long* ctx) {
}

/** activate idle thread */
void Scheduler::idle() {
  Thread::startAllThreads();
  Timer::start();
  // while(1) sleep(0xffffffff); // while(1) becouse timerinterups terminaes the sleep
  // FreeRTOS
  vTaskStartScheduler();

}

// Not used in FreeRTOS, becouse FreeRTOS hast its own schedulder
// void Scheduler::schedule() { }

// TODO should we use here the idleHook, in order to see that the scheduler is alive?
// Another option is to use the TickCounter
unsigned long long Scheduler::getScheduleCounter() {
  return scheduleCounter;
}


/** returns true if scheduler is considered to be running */
bool Scheduler::isSchedulerRunning() {

  bool retValue = false;
  portBASE_TYPE schedulerState;
  // FreeRTOS 
  schedulerState = xTaskGetSchedulerState();
  
  if (schedulerState == taskSCHEDULER_RUNNING) {
    retValue = true;
  }
  return retValue;

}



#ifndef NO_RODOS_NAMESPACE
}
#endif

