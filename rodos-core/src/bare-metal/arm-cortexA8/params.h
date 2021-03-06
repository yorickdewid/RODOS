

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
* @date 2013/03/12 17:30
* @author Lutz Dittrich, Sergio Montenegro, Tobias Mikschl
*
* Copyright 2008 DLR
*
* @brief configuration parameters ...
*
*/

#ifndef __PARAMS_H__
#define __PARAMS_H__

//#include "utils/uartstdio.h"

/*************** Version Number ***************/
#define OSVERSION "AM335X ARM-CortexA8 V1.0"
//#define printf UARTprintf
//#define putchar UARTputchar



/*************** System Configuration *********/
/** the following define sets the UART used for debug outputs with xprintf
 *
 */
#ifndef UART_DEBUG
#define UART_DEBUG			UART_IDX0 //RODOS boot messages will always go to UART0 until scheduler is running
#endif
/** Memory for allocation (xmalloc) eg for all stacks ***/
#define XMALLOC_SIZE		80*1024	/* = 20480 * 100B = 2,048 MB */

/** default stack size (in bytes) for threads */
#define DEFAULT_STACKSIZE       2048	/* 8 kB */

/** stack size (in bytes) for scheduler threads */
#define SCHEDULER_STACKSIZE 	1024

/** time interval between timer interrupts in microseconds */
#define PARAM_TIMER_INTERVAL	1000
/*** time for time slice to swtich between threads with same priority ***/
#define  TIME_SLICE_FOR_SAME_PRIORITY (100*MILLISECONDS)

/** default priority for newly created threads */
#define DEFAULT_THREAD_PRIORITY	100
/** user threads shall not have a priority higher than this */
#define MAX_THREAD_PRIORITY	1000

/** high priority levels for priority ceiling  */
#define NETWORKREADER_PRIORITY	(MAX_THREAD_PRIORITY + 2)
/** high priority levels for priority ceiling  */
#define CEILING_PRIORITY	(NETWORKREADER_PRIORITY + 1)

/** using a network, the maximal number of nodes attached */
#define MAX_NUMBER_OF_NODES	10
/** if using network it may have a limit for pakets, eg udp 1500 */
#define MAX_NETWORK_MESSAGE_LENGTH	1300


/*** If using a network: Maximal number of subscribers per node */
#define MAX_SUBSCRIBERS		60

#undef  stdout
#define stdout 
#define fflush nop

#endif /* __PARAMS_H__ */
