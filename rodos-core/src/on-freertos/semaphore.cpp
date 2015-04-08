

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
*
* @brief %Semaphore - a synchronisation primitive (Wrapper for FreeRTOS)
*
* binary semaphore, mutex
*
*/


#include "semaphore.h"

// FreeRTOS includes
#include "FreeRTOS.h"  // Contains type specifications of FreeRTOS
#include "semphr.h"    // Semaphore and Mutexes


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

// TODO Do we still need this. It is used in cases where the priority cannot be switched.
// A global mutex intended to ceil ppriority
static xSemaphoreHandle xGlobalMutex;

// Take the mutex exclusively
void globalAtomarLock() {
   xSemaphoreTake(xGlobalMutex, portMAX_DELAY);
}

// Give mutex free to others.
void globalAtomarUnlock() {
   xSemaphoreGive(xGlobalMutex);
}

/*****************************/


/**
 *  Constructor
 */
Semaphore::Semaphore() :
  owner(0), ownerPriority(0), context(0), ownerEnterCnt(0) { 
  
  // Create a mutex and store the handle in context.
  xSemaphoreHandle xMutex = xSemaphoreCreateMutex();
  this->context = (void *) xMutex;

  // TODO Dow we need this?
  xGlobalMutex = xSemaphoreCreateMutex();
}


/**
* Caller will be blocked if semaphore is occupied.
* Owner may reenter without deadlock.
*/
void Semaphore::enter() {
  Thread* caller = Thread::getCurrentThread();
  if (owner == caller) {
    // We are allready within the semaphore.
    ownerEnterCnt++;
    return;
  }
  // Get the mutex handle and take semaphore with now timeout
  xSemaphoreHandle xMutex = (xSemaphoreHandle) this->context;
  xSemaphoreTake(xMutex, portMAX_DELAY); 
  // TODO Should we handle the xcase where the mutex is not obtained (!= pdPASS) ?
  owner = caller;
  ownerEnterCnt = 1;
}

/**
*  Caller does not block. Resumes one waiting thread (enter)
*/
void Semaphore::leave() {
  ownerEnterCnt--;
  if (ownerEnterCnt > 0) {
    // Semaphore is still held by the same owner
    return;
  }
  // ownerCnt == 0
  owner = 0;

  // Get the mutex handle and take semaphore with now timeout
  xSemaphoreHandle xMutex = (xSemaphoreHandle) this->context;
  xSemaphoreGive(xMutex); 
}


#ifndef NO_RODOS_NAMESPACE
}
#endif


