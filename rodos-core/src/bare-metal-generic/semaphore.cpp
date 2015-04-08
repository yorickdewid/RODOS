
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
 * @file semaphore.cc
 * @date 2008/05/11 10:12
 * @author Sergio Montenegro
 *
 * Copyright 2008 DLR
 *
 * @brief %Semaphore - a synchronisation primitive
 *
 * binary semaphore, mutex
 *
 */

#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 *  Constructor
 */
Semaphore::Semaphore() :
  owner(0), ownerPriority(0), context(0), ownerEnterCnt(0) { }

/**
 * caller will be blocked if semaphore is occupied
 * The owner may reenter the semaphore without deadlock
 */
void Semaphore::enter() {
  Thread* caller = Thread::getCurrentThread();
  long callerPriority = caller->getPriority();
  PRIORITY_CEILING {
    // Check if semaphore is occupied by another thread
    if ((owner != 0) && (owner != caller) ) {

      // Avoid priority inversion
      if (callerPriority > owner->getPriority()) {
        owner->setPriority(callerPriority);
      }
      // Sleep until wake up by leave
      while(owner != 0 && owner != caller) Thread::suspendCallerUntil(END_OF_TIME, this);
      ownerEnterCnt = 0;
    }
    owner = caller;
    ownerPriority = callerPriority;
    ownerEnterCnt++;
  } // end of prio_ceiling
  caller->yield(); // wating with prio_ceiling, maybe some one more important wants to work?
}

/**
 *  caller does not block. resumes one waiting thread (enter)
 */
void Semaphore::leave() {
  Thread* caller = Thread::getCurrentThread();
  Thread* waiter = 0;

  if (owner != caller) { // User Programm error: What to do? Nothing!
    return; 
  }
 
  ownerEnterCnt--;
  if (ownerEnterCnt > 0) { // same thread made multiple enter()
    return;
  }

  PRIORITY_CEILING {
	// - priority of current thread might have been increased in enter() due to a semaphore access
	//   of another thread with higher priority
	// - to restore old priority of current thread we set _previusPriority to the old value saved in ownerPriority
	// - priority of current thread will be set to _previusPriority-1 when leaving PRIORITY_CEILING
    _previusPriority = ownerPriority + 1; // _previusPriority is defined & used in PRIORITY_CEILING, it substracts 1

    ownerPriority = 0;
    owner = 0;
    waiter = Thread::findNextWaitingFor(this);

    if (waiter != 0) {
      owner = waiter; // set new owner, so that no other thread can grep the semaphore before thread switch
      waiter->resume();
    }
  } // end of PRIORITY_CEILER, Restores prio set in _previusPriority

  /* 
   * In case resume perfomes no yield:
   * To speed up semaphore handling the waiter-thread should be executed asap
   * -> that can be achieved by pause caller-thread with yield()
   * If there is a scheduler-call (triggered by timer) before the yield-call, it is not
   * necessary any more!
   * -> to avoid unnecessary yield the variable ownerPriority will be checked
   * -> if ownerPriority is still 0 there was no thread-switch before
   * -> Of course there can be a thread switch directly before the yield-call. In this case
   *    we have to accept the delay caused by the unnecessary yield-call.
   */
  if ( (waiter != 0) && (ownerPriority == 0) ) { caller->yield(); }
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

