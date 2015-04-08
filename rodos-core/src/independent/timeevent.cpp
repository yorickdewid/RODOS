

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
* @file event.cc
* @date 2008/05/06 10:12
* @author Sergio Montenegro, Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief %Event management
*/

#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/* constructor */
TimeEvent::TimeEvent(const char* name) : ListElement(TimeEvent::timeEventList,name) {
    eventAt     = END_OF_TIME;
    eventPeriod = 0; 
}


/* destructor */
TimeEvent::~TimeEvent() {
    ERROR("Time EventHandler deleted");
}

/* Sets the time when the handler should be called
 * @param absolute time of next event
*/
void TimeEvent::activateAt(const int64_t time) {
    eventAt = time;
    eventPeriod = 0;
}

/* defines the time relative to now, when the handler should be called: DEPRECATED */
// void TimeEvent::activateIn(const int64_t time) { activateAt(NOW() + time); }

/* like activateAt but with periodic reactivation */
void TimeEvent::activatePeriodic(const int64_t startAt, const int64_t period) {
    eventPeriod = period;
    eventAt = startAt;
}


/** TBA   Invoke event handler. Events are simply invoked by comparing event time and system time.
* calls all time event handlers which eventAt < now (past)
* and updates eventAt.
* Returns the number of handle() called
*/
int32_t TimeEvent::propagate(const int64_t timeNow) {
    int32_t cnt = 0;
    ITERATE_LIST(TimeEvent, TimeEvent::timeEventList) {
        if (iter->eventAt < timeNow) {
            if (iter->eventPeriod == 0) { // not again until user sets it again
                iter->eventAt = END_OF_TIME;
            } else {
                iter->eventAt+= iter->eventPeriod;
                if (iter->eventAt < timeNow) { // Still in the past?
                    iter->eventAt = TimeModel::computeNextBeat(iter->eventAt,
                                    iter->eventPeriod,
                                    timeNow);
                }
            }
            iter->handle();
            cnt++;
        }
    }
    return cnt;
}


/* call init for each element in list */
int32_t TimeEvent::initAllElements() {
    int32_t cnt = 0;
    ITERATE_LIST(TimeEvent, TimeEvent::timeEventList) {
        iter->init();
        cnt++;
    }
    return cnt;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

