/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief RODOS related configuration parameters.
 * This file holds RODOS specific configuration parameters.
 * Architecture specific parameters are located in include/param.h
 */

#ifndef _SPARC64_PARAMS_H
#define _SPARC64_PARAMS_H

#include <param.h>

/**
 * @defgroup hwVers Version constants.
 */
/**@{*/

/** version number shown in applications */
#ifndef OSVERSION
#define OSVERSION                     "RODOS-sparc64-1"
#endif /* OSVERSION */

/**
 * SILO bootloader specific:
 * Kernel version shown by SILO with the format: XX.YY.ZZ
 * To calculate the version code, use XX*65536 + YY*256 + ZZ*1
 */
#define SILO_VERSION_CODE             1*256

/**@}*/

/**
 * @defgroup hwRodos RODOS configuration.
 */
/**@{*/

/** memory for allocation (xmalloc) eg for all stacks */
#ifndef XMALLOC_SIZE
#define XMALLOC_SIZE                  (PAGE_SIZE*256) /* 0x200000 = 2M  */
#endif /* XMALLOC_SIZE */

/** default stack size (in bytes) for threads */
#ifndef DEFAULT_STACKSIZE
#define DEFAULT_STACKSIZE             (PAGE_SIZE*2) /* 0x4000 = 16k */
#endif /* DEFAULT_STACKSIZE */

/** default stack size (in bytes) for the scheduler
 * Whenever passing control to the scheduler we could switch to the provided
 * __schedulerStack__. But we already have a boot stack that would be doomed
 * and never be used again. This is wasteful, so at the moment we continue to
 * use the boot stack again whenever jumping to the scheduler.
 */
#ifndef SCHEDULER_STACKSIZE
#define SCHEDULER_STACKSIZE           0 /* unused */
#endif /* SCHEDULER_STACKSIZE */

/** time interval between timer interrupts in microseconds.
 * In the RODOS main(), Timer::setInverval(PARAM_TIMER_INTERVAL) is called.
 */
#ifndef PARAM_TIMER_INTERVAL
#define PARAM_TIMER_INTERVAL          1000
#endif /* PARAM_TIMER_INTERVAL */

/** time slice to switch between threads with priority conflicts */
#ifndef TIME_SLICE_FOR_SAME_PRIORITY
#define TIME_SLICE_FOR_SAME_PRIORITY  (100*MILLISECONDS)
#endif /* TIME_SLICE_FOR_SAME_PRIORITY */

/** default priority for newly created threads */
#ifndef DEFAULT_THREAD_PRIORITY
#define DEFAULT_THREAD_PRIORITY       100
#endif /* DEFAULT_THREAD_PRIORITY */

/** user threads shall not have a priority higher than this */
#ifndef MAX_THREAD_PRIORITY
#define MAX_THREAD_PRIORITY           1000
#endif /* MAX_THREAD_PRIORITY */

/** high priority levels for priority ceiling  */
#ifndef NETWORKREADER_PRIORITY
#define NETWORKREADER_PRIORITY        (MAX_THREAD_PRIORITY + 2)
#endif /* NETWORKREADER_PRIORITY */

/** high priority levels for priority ceiling  */
#ifndef CEILING_PRIORITY
#define CEILING_PRIORITY              (NETWORKREADER_PRIORITY + 1)
#endif /* CEILING_PRIORITY */

/** if using a network, the maximal number of nodes attached */
#ifndef MAX_NUMBER_OF_NODES
#define MAX_NUMBER_OF_NODES           10
#endif

/** if using network it may have a limit for pakets, eg udp 1500 */
#ifndef MAX_NETWORK_MESSAGE_LENGTH
#define MAX_NETWORK_MESSAGE_LENGTH    1300
#endif /* MAX_NETWORK_MESSAGE_LENGTH */

/** if using a network, the maximal number of subscribers per node */
#ifndef MAX_SUBSCRIBERS
#define MAX_SUBSCRIBERS               60
#endif /* MAX_SUBSCRIBERS */

/**@}*/

#endif /* _SPARC64_PARAMS_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
