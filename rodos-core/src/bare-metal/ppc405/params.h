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
*/

#ifndef RODOS_HW_PARAMS_H
#define RODOS_HW_PARAMS_H

/** Version Number */
#define OSVERSION "RODOS-PPC405"

// /*************** System Configuration *********/

/** Memory for allocation (xmalloc) eg for all stacks ***/
#ifndef XMALLOC_SIZE
#define XMALLOC_SIZE                  500000
#endif

/** default stack size (in bytes) for threads */
#ifndef DEFAULT_STACKSIZE
#define DEFAULT_STACKSIZE             10000
#define SCHEDULER_STACKSIZE DEFAULT_STACKSIZE
#endif

/** time interval between timer interrupts in microseconds. */
#ifndef PARAM_TIMER_INTERVAL
#define PARAM_TIMER_INTERVAL          10000
#endif

/*** time for time slice to swtich between threads with same priority ***/
#define TIME_SLICE_FOR_SAME_PRIORITY  (100*MILLISECONDS)

/** default priority for newly created threads */
#ifndef DEFAULT_THREAD_PRIORITY
#define DEFAULT_THREAD_PRIORITY       100
#endif

/** user threads shall not have a priority higher than this */
#ifndef MAX_THREAD_PRIORITY
#define MAX_THREAD_PRIORITY           1000
#endif

/** high priority levels for priority ceiling  */
#ifndef NETWORKREADER_PRIORITY
#define NETWORKREADER_PRIORITY        (MAX_THREAD_PRIORITY + 2)
#endif

/** high priority levels for priority ceiling  */
#ifndef CEILING_PRIORITY
#define CEILING_PRIORITY              (NETWORKREADER_PRIORITY + 1)
#endif

/** using a network, the maximal number of nodes attached */
#ifndef MAX_NUMBER_OF_NODES
#define MAX_NUMBER_OF_NODES           10
#endif

/** if using network it may have a limit for pakets, eg udp 1500 */
#ifndef MAX_NETWORK_MESSAGE_LENGTH
#define MAX_NETWORK_MESSAGE_LENGTH    1300
#endif

// /*** If using a network: Maximal number of subscribers per node */
#ifndef MAX_SUBSCRIBERS
#define MAX_SUBSCRIBERS               60
#endif

#endif /* RODOS_HW_PARAMS_H */

