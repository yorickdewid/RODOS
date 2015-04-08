

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
* @file params.h
* @date 2008/04/24 16:13
* @author Lutz Dittrich, Sergio Montenegro
*
* Copyright 2008 DLR
*
* @brief configuration parameters ...
*
*/

#ifndef __PARAMS_H__
#define __PARAMS_H__

// #include "hw_datatypes.h"
#include <semaphore.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/** Version Number */
#define OSVERSION "RODOS-79-virtex-xilkernel5"

/*************** System Configuration *********/

/** Clock constants */
#define SYSTEM_CLOCK_IN_MHZ 100
#define NANOSECONDS_PER_CLOCK_TICK 10

/** Base address of Xtimer with interrupt in Virtex hardware design */
#define XTIMER_BASE 0x83c00000

/** time interval between timer interrupts in microseconds */
#define PARAM_TIMER_INTERVAL   1000

/** Optionally insert additional \r for all \n */
#define PUTCHAR_INSERTS_CARRIAGE_RETURNS

/** default stack size (in bytes) for threads */
#define DEFAULT_STACKSIZE       2000

/** default priority for newly created threads */
#define DEFAULT_THREAD_PRIORITY                    100

/** user threads shall not have a priority higher than this */
#define MAX_THREAD_PRIORITY		       1000000

/** high priority levels for priority ceiling  */
#define NETWORKREADER_PRIORITY	   	(MAX_THREAD_PRIORITY + 2)

/** high priority levels for priority ceiling  */
#define CEILING_PRIORITY			(NETWORKREADER_PRIORITY + 1)

/** Memory for allocation (xmalloc) eg for all stacks ***/
#define XMALLOC_SIZE		2000000



/** using a network, the maximal number of nodes attached */
#define MAX_NUMBER_OF_NODES		10
/** if using network it may have a limit for pakets, eg udp 1500 */
#define MAX_NETWORK_MESSAGE_LENGTH	1300


/*** If using a network: Maximal number of subscribers per node */
#define MAX_SUBSCRIBERS			60


/*** Thread's semaphore to implement wait with resume 
     This is necessary as Xilkernel doesn't support this feature
     in a native way */
class VirtexSemaphore {

 public: 
    sem_t sem;

    VirtexSemaphore() {
    }
    
    void init () {
        sem_init(&sem,0,0);
    }

    ~VirtexSemaphore() {
        sem_destroy(&sem);
    }
};


#define HW_THREAD_MORE public: VirtexSemaphore virtex_sem;


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __PARAMS_H__ */
