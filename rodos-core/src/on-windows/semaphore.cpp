

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

#include "semaphore.h"

#include <windows.h>
#include <stdio.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

CRITICAL_SECTION g_plSEMA_CriticalSection;
static bool g_bSEMA_CriticalSection_FirstTime = true;
static long g_lSemaCount = 0;

void globalAtomarLock_Init (void)
{
  g_bSEMA_CriticalSection_FirstTime = false;
  InitializeCriticalSection (&g_plSEMA_CriticalSection);
}

void globalAtomarLock() 
{
  if (g_bSEMA_CriticalSection_FirstTime)
    globalAtomarLock_Init();

  EnterCriticalSection (&g_plSEMA_CriticalSection);
}

void globalAtomarUnlock() 
{
  if (g_bSEMA_CriticalSection_FirstTime)
    globalAtomarLock_Init();

  LeaveCriticalSection (&g_plSEMA_CriticalSection);
}

/*****************************/

/**
 *  Constructor
 */

#define SEMAPHORE_START_VALUE      1
#define SEMAPHORE_MAX_VALUE      255
#define SEMAPHORE_RELEASE_COUNT    1


Semaphore::Semaphore() 
: owner(0), ownerPriority(0), ownerEnterCnt(0) 
{ char achName[256];

  sprintf (achName, "RODOS_Semaphor_%ld", g_lSemaCount++);
  context = CreateSemaphore(NULL, SEMAPHORE_START_VALUE, SEMAPHORE_MAX_VALUE, achName);
}

// Never called
//Semaphore::~Semaphore() 
//{
//  CloseHandle ((HANDLE) context);
//}

/**
* caller will be blocked if semaphore is occupied
* Owner may reenter withput deadlock
*/
void Semaphore::enter()
{ 
  WaitForSingleObject ((HANDLE) context, INFINITE);
}

/**
*  caller does not block. resumes one waiting trhead (enter)
*/
void Semaphore::leave() 
{
  ReleaseSemaphore ((HANDLE) context, SEMAPHORE_RELEASE_COUNT, NULL);
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

