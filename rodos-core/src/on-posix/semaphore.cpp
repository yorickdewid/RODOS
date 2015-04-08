

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

#include <unistd.h>
#include <pthread.h>
#include <signal.h>
// #include <stdlib.h>


#include "rodos.h"

namespace RODOS {
pthread_mutex_t pthreadMutexInitialisation = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t globalAtomar = PTHREAD_MUTEX_INITIALIZER;


void globalAtomarLock() {
  pthread_mutex_lock(&globalAtomar);
}
void globalAtomarUnlock() {
  pthread_mutex_unlock(&globalAtomar);
}
/*****************************/


/**
 *  Constructor
 */
Semaphore::Semaphore() :
  owner(0), ownerPriority(0), ownerEnterCnt(0) {

	pthread_mutex_t *mutexp = new pthread_mutex_t;
	*mutexp = pthreadMutexInitialisation;
	context = (void*)mutexp;
}


/**
* caller will be blocked if semaphore is occupied
* Owner may reenter withput deadlock
*/
void Semaphore::enter() {
  Thread* caller = Thread::getCurrentThread();
  if(owner == caller) {
	ownerEnterCnt++;
	return;
  }
  pthread_mutex_t *mutexp = (pthread_mutex_t*)context;
  pthread_mutex_lock(mutexp);
  owner =  caller;
  ownerEnterCnt = 1;
}

/**
*  caller does not block. resumes one waiting trhead (enter)
*/
void Semaphore::leave() {
  ownerEnterCnt--;
  if(ownerEnterCnt != 0) return;
  owner = 0;
  pthread_mutex_t *mutexp = (pthread_mutex_t*)context;
  pthread_mutex_unlock(mutexp);
  
}
}
