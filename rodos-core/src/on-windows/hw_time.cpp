

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
* @author Lutz Dittrich, Sergio Montenegro
*
* Copyright 2008 DLR
*
* @brief functions for time management
*/

//#include <time.h>
#include <windows.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/***************** Get Time functions **************/

// static long long timeAtStartup = 0LL;  now defined at 00globalobjects
extern long long timeAtStartup; 

static long long g_llGetTime_TimerFreq = 1;
static long long g_llGetTime_Factor    = 1;

unsigned long long hwGetNanoseconds()
{ LARGE_INTEGER llPerformanceCount;
  long long llSec, llNSec;

  QueryPerformanceCounter   (&llPerformanceCount);
  llSec  = (llPerformanceCount.QuadPart / g_llGetTime_TimerFreq) * 1000000000;
  llNSec = (llPerformanceCount.QuadPart % g_llGetTime_TimerFreq) * g_llGetTime_Factor;

  return llSec + llNSec;
}

void hwInitTime() 
{ LARGE_INTEGER llFreq;
  
  QueryPerformanceFrequency (&llFreq);
  g_llGetTime_TimerFreq = llFreq.QuadPart;
  g_llGetTime_Factor    = 1000000000 / g_llGetTime_TimerFreq;

  timeAtStartup = hwGetNanoseconds();
}

/******* abslute time (from host) for exotic function eg.random generator **/

unsigned long long hwGetAbsoluteNanoseconds() {
	return hwGetNanoseconds() + timeAtStartup;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

