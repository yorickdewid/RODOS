

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
* @file semaphore.h
* @date 2008/05/10 16:37
* @author Sergio Montenegro
*
* Copyright 2008 DLR
*
* @breif binary semaphore, mutex a synchronisation primitive 
*/

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "thread.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
* @class Semaphore
* @brief a synchronisation primitive
*
* Mutex Semaphore.
* the next thread to be activated is
* selected according to priorities and
* not come first serve first.
*
*/

class Semaphore {

private:
  Thread* owner;
  long ownerPriority;
  void* context; ///< used only on posix and on host-os
  int ownerEnterCnt;

public:
  /** Constructor */
  Semaphore();
  // ~Semaphore() { } // Shall never be called. Semaphores may not disapear

  /** caller will be blocked if semaphore is occupied 
   ** The owner may reenter the semaphore without deadlock */
  void enter();

  /** caller does not block. resumes one waiting thread (enter) */
  void leave();

  /** true if semaphore is free:
  *   Warning: next it can be occupied by someone else
  */
  // bool isFree();
 
  /// This ist jus a help für the macro PROTECT_WITH_SEMAPHORE
  inline bool enterRetTrue() { enter(); return true; }
};

#define PROTECT_WITH_SEMAPHORE(sem) \
  for(bool _onlyonce_ = sem.enterRetTrue(); _onlyonce_ ; _onlyonce_ = false, sem.leave()) 


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __SEMAPHORE_H__ */
