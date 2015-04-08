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
 * @file event.h
 * @date 2008/05/01 16:15
 * @author Sergio Montenegro, Lutz Dittrich
 *
 * Copyright 2008 DLR
 *
 * Different events handlers
 *
 * @brief %Event management (header)
 */

#pragma once

#include "listelement.h"
#include "timemodel.h"
#include "debug.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/**
 * @class TimeEvent
 * @brief %Time management
 *
 * React on and handle time events
 **/

class TimeEvent: public ListElement {

  friend void initSystem();

protected:
  /// default list of all time events
  static List timeEventList;

  /// Time, when the event is activated to handle
  int64_t eventAt;

  /**
   * Periodical activation time of the event. If set to 0, the event is activated only once,
   * else it is activated with a period of this time.
   */
  int64_t eventPeriod;

public:

  /**
   * Constructor
   * @param[in] name Name of the event.
   */
  TimeEvent(const char* name = "TimeEvent");

  /**
   * Pure virtual destructor.
   */
  virtual ~TimeEvent();

  static int32_t initAllElements();

  /**
   * Calls all time event handlers which eventAt < now (past)
   * and updates eventAt.
   *
   * @warning the execution order of event handlers for one timeslot
   *   is NOT guaranteed. If events A and B have similar schedule times with
   * t(A) < t(B) it could happen, that B is handled before A.
   *
   * @return the number of handle() called
   */
  static int32_t propagate(const int64_t timeNow);

  /**
   * Defines the time point when the method handle will be called one time.
   *
   * @param time Time to activate time event.
   */
  void activateAt(const int64_t time);

  /**
   * Like activateAt, but will be reactivated cyclic with the given period.
   *
   * @param startAt First time when the time event is activated.
   * @param period The gap time between two activations of the time event.
   */
  void activatePeriodic(const int64_t startAt, const int64_t period);

  /**
   * Handle the event. The default implementation of handle search for all threads
   * waiting on this event. This is the case, if the thread calls Thread::suspendCallerUntil
   * with parameter signaler a pointer to this event. The user can overload the method with
   * an own processing of the event.
   *
   * @warning 1
   * Handle is running in the context of the caller, maybe an interrupt handler.
   * So do it very short and avoid blocking functionalities inside the implementation of handle
   * like semaphores, IO activities and so on.
   *
   * @see Thread::suspendCallerUntil
   */
  virtual void handle(void) { }

  /**
   * The method is called once at system init during start up of the system.
   */
  virtual void init(void) { }

};


#ifndef NO_RODOS_NAMESPACE
}
#endif

