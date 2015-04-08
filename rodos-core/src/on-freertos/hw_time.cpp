

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
* @file hw_time.cc
* @date 2008/06/04 10:19
*
*
* @brief functions for time management
*/

//#include <time.h>
// Rodos includes
#include "timemodel.h"

// FreeRTOS includes
#include "FreeRTOS.h"  // Contains type specifications of FreeRTOS
#include "task.h"      // Tasks and Scheduler


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/***************** Get Time functions **************/

// static long long timeAtStartup = 0LL;  now defined at 00globalobjects
extern long long timeAtStartup; 

/// Gets system time in ticks and converts to nanoseconds
long long hwGetNanoseconds() {
  /** Note: tickCount is of type portTickType which 
   * is a 32bit variable of type unsigned long defined in portmacro.h.
   * With a rate of 50 Hz this covers at most 2.77 years or 2 years and 9 months.
   */
  return ((TTime) xTaskGetTickCount() * (TTime) portTICK_RATE_MS * MILLISECONDS );
}


void hwInitTime() {
  timeAtStartup = hwGetNanoseconds();
}


/******* abslute time (from host) for exotic function eg.random generator **/

long long hwGetAbsoluteNanoseconds() {
	return hwGetNanoseconds() + timeAtStartup;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

