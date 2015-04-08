

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

#include "hw_specific.h"
#include "timeevent.h"
#include "thread.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

// ***************************************************************************

extern volatile long taskRunning;
long long Timer::microsecondsInterval = 0;

class DummyTimerThread : public Thread
{
  public:
    long m_lInterval_ms;
    bool m_bIsRunning;

    ~DummyTimerThread() {}
    DummyTimerThread() : Thread ("MainTimerDumyThread", 0x33, 0x100000)
//                               (WIN_THREAD_PRIORITY_HIGH << 4) | WIN_THREAD_PRIORITY_HIGH, 0x100000) 
    { m_lInterval_ms = 10; 
      m_bIsRunning   = false;
    }

  public:
    void put(const long topicId, const int msgLen, const void* msg, long linkId) {}
    void put(const long topicId, const int msgLen, const void* msg) {}
    void init() {}
    void run() 
    { TTime llNow = NOW();
      
      while (1)
      { llNow += ((long long) m_lInterval_ms) * 1000000;
        if (llNow < NOW()) continue;
        Thread::suspendCallerUntil (llNow);

        if (m_bIsRunning)
          TimeEvent::propagate(NOW());
      }
    }

};

static DummyTimerThread *m_pcDummyTimerThread = 0;

// ***************************************************************************

/**
* initialize the timer and signal handler 
*/
void Timer::init() 
{
  m_pcDummyTimerThread = new DummyTimerThread();
}

/**
* start timer 
*/
void Timer::start() 
{
  if (m_pcDummyTimerThread)
    m_pcDummyTimerThread->m_bIsRunning = true;
}

/**
* stop timer 
*/
void Timer::stop() 
{
  if (m_pcDummyTimerThread)
    m_pcDummyTimerThread->m_bIsRunning = false;
}

/**
* set timer interval 
*/
void Timer::setInterval(const long long microsecondsInterval) 
{ long lMS = (long) (microsecondsInterval / 1000);

  lMS = (lMS < 1) ? 1 : lMS;

  if (m_pcDummyTimerThread)
    m_pcDummyTimerThread->m_lInterval_ms = lMS;
 
  Timer::microsecondsInterval = microsecondsInterval;
}

// SetTimer ((HWND) 0, (UINT_PTR) 0, (UINT) 100, (TIMERPROC) timerSignalHandler);
// CreateWaitableTimer (0, 0, "TestTimer");
// SetWaitableTimer
// CancelWaitableTimer 


#ifndef NO_RODOS_NAMESPACE
}
#endif

