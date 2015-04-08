

/*********************************************************** Copyright
 **
 ** Copyright (c) 2010, German Aerospace Center (DLR)
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
* @author Lutz Dittrich, Sergio Montenegro,
*         adapted in 2010 by Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2008 DLR
*
* @brief Some configuration parameters...
*
* More hardware specific configuration options can be found in hw_config.h
*/

#ifndef RODOS_HW_PARAMS_H
#define RODOS_HW_PARAMS_H

/** Version Number */
#define OSVERSION "RODOS-BFIN"
///\todo HW specifig parameer
// /*************** System Configuration *********/

/** Memory for allocation (xmalloc) eg for all stacks ***/
#ifndef XMALLOC_SIZE
#define XMALLOC_SIZE		    4194304//4MB alligned for chach
#endif

/** default stack size (in bytes) for threads */
#ifndef DEFAULT_STACKSIZE
#define DEFAULT_STACKSIZE       16384//aligned for cache
#define SCHEDULER_STACKSIZE DEFAULT_STACKSIZE
#endif

/** time interval between timer interrupts in microseconds. */
#ifndef PARAM_TIMER_INTERVAL
#define PARAM_TIMER_INTERVAL    10000
#endif

/*** time for time slice to swtich between threads with same priority ***/
#define  TIME_SLICE_FOR_SAME_PRIORITY (100*MILLISECONDS)

/** default priority for newly created threads */
#ifndef DEFAULT_THREAD_PRIORITY
#define DEFAULT_THREAD_PRIORITY 100
#endif

/** user threads shall not have a priority higher than this */
#ifndef MAX_THREAD_PRIORITY
#define MAX_THREAD_PRIORITY		1000
#endif

/** high priority levels for priority ceiling  */
#ifndef NETWORKREADER_PRIORITY
#define NETWORKREADER_PRIORITY	(MAX_THREAD_PRIORITY + 2)
#endif

/** high priority levels for priority ceiling  */
#ifndef CEILING_PRIORITY
#define CEILING_PRIORITY		(NETWORKREADER_PRIORITY + 1)
#endif

/** using a network, the maximal number of nodes attached */
#ifndef MAX_NUMBER_OF_NODES
#define MAX_NUMBER_OF_NODES		    10
#endif

/** if using network it may have a limit for pakets, eg udp 1500 */
#ifndef MAX_NETWORK_MESSAGE_LENGTH
#define MAX_NETWORK_MESSAGE_LENGTH  1300
#endif

// /*** If using a network: Maximal number of subscribers per node */
#ifndef MAX_SUBSCRIBERS
#define MAX_SUBSCRIBERS				60
#endif


#endif /* RODOS_HW_PARAMS_H */
