

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

#include "timemodel.h"    // RODOS specific time definitions
#include "hw_specific.h"  // used for timer class declaration
#include "timeevent.h"        // time events use by timer function

// FreeRTOS includes
#include "FreeRTOS.h"  // Contains type specifications of FreeRTOS
#include "timers.h"    // FreeRTOS timer library


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


extern volatile long taskRunning; // timer period, set by caller

/**
* Size of stack required by signal handler.
*/
#define SIGNAL_HANDLER_STACK_SIZE SIGSTKSZ

/** the timer interval */
long long Timer::microsecondsInterval = portMAX_DELAY;
static xTimerHandle xTimer; // Handle for timer 

/**
 * The signal handler for SIGVTALRM (timer signal).
 * The function parameter is required, but not used.
*/
void timerSignalHandler(xTimerHandle ignore) {
  
    // time events to call?
    TTime timeNow = NOW();  // comment this out to improve performance, but: no time events any more
    TimeEvent::propagate(timeNow); // comment this out to improve performance, but: no time events any more
}

/**
 * Initialize the timer and set signal handler.
 * In FreeRTOS the timer is based on a timer task, 
 * set periodically to the blocked state.
 */
void Timer::init() {

  // TODO Move this to the class and define them as static vars
  // Set the period to a max number, it will be set before the timer is started.
  portTickType xTimerPeriod = portMAX_DELAY; 
  long vDummyTimerID = 0; // Timer ID, not used  

  xTimer = 
    xTimerCreate( ( signed char *) "FreeRTOSTimer", // timer name
		  xTimerPeriod,             // timer period in ticks
		  (unsigned portBASE_TYPE) pdTRUE, // pdTRUE to set timer periodically
		  (void *) vDummyTimerID, // ID to distiguish timers if there is more than one timer
		  (tmrTIMER_CALLBACK) timerSignalHandler); // handler called from timer function 

}


/**
 * Start timer. Recalculate period according to the period set most recently. 
 */
void Timer::start() {
  
  // Convert time from microseconds to ticks (about 20 milliseconds)
  portTickType xTimerPeriod = (portTickType) microsecondsInterval / (1000 * portTICK_RATE_MS);
  // TODO Set block time to timer period /indefinetely?
  // portTickType xBlockTime = (portTickType) microsecondsInterval / (1000 * portTICK_RATE_MS);
  portTickType xBlockTime = portMAX_DELAY;
  
  /* Use xTimerChangePeriod instead of xTimerStart, since we must assume that
   * the period has been changed since initialization. 
   * The timer will expire after the time period counted from this call.
   */
  // TODO if (xTimerStart(xTimer, xEventTimerStart) == pdFALSE)
  if (xTimerChangePeriod(xTimer, xTimerPeriod, xBlockTime) == pdFALSE) {
    ERROR("Timer could not be started!\n");
    FFLUSH();
  }
}


/**
 * Stop timer. Set blocking time, so the timer will wait indefinetely. 
 */
void Timer::stop() {
  
  xTimerStop(
	     xTimer,          // handle to timer
	     portMAX_DELAY);  // timer will wait indefinitely
}

/**
 * Set timer interval. 
 */
void Timer::setInterval(const long long microsecondsInterval) {
  Timer::microsecondsInterval = microsecondsInterval;
}



#ifndef NO_RODOS_NAMESPACE
}
#endif

