
/**
 * @file params.h
 * @date 2012/08/20 16:13
 * @author Michael Ruffer
 *
 * Copyright 2012 University Wuerzburg
 *
 * @brief configuration parameters ...
 *
 */

#ifndef __PARAMS_H__
#define __PARAMS_H__

#include "userconfig.h"

#ifdef __cplusplus
#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif
#endif

/** Version Number */
#define OSVERSION "STM32F4 Cortex-M4 V1.0"

/*************** System Configuration *********/

/** system clocks
 * - APB1_CLK is fixed to CPU_CLK/4 (TIM2/3/4/5/6/7/12/13/14, USART2/3, UART4/5, SPI2/3, I2C1/2/3, CAN1/2, DAC1/2, WWDG)
 * - APB2_CLK is fixed to CPU_CLK/2 (TIM1/8/9/10/11, USART1/6, SPI1, ADC1/2/3, MMC)
 * - for more information see system_stm32f4xx.c
 */
#ifdef STM32F40_41xxx   // MINI-M4, STM32F4Discovery
#define CPU_CLK                         168   // 24,32,42,48,56,84,96,120,144,168 MHz
#endif
#ifdef STM32F429_439xx  // STM32F429Discovery
#define CPU_CLK                         180   // 24,32,42,48,56,84,96,120,144,168,180 MHz
#endif
#ifdef STM32F427_437xx
#define CPU_CLK                         168   // 24,32,42,48,56,84,96,120,144,168,180 MHz
#endif
// STM32F4xx_xxxx is set in RODOS_ROOT/make/stm32f4-set-vars or eclipse project settings


/** the following define sets the UART used for debug outputs with xprintf
 *
 */
#ifndef UART_DEBUG
#define UART_DEBUG                      UART_IDX3, GPIO_056, GPIO_057 // PD5 and PD6
//#define UART_DEBUG                      UART_IDX2, GPIO_053, GPIO_054 // PD5 and PD6
//#define UART_DEBUG                      UART_IDX4, GPIO_000, GPIO_001 // PA0 and PA1
//#define UART_DEBUG                      UART_IDX5
#endif

/** Memory for allocation (xmalloc) e.g. for all thread stacks ***/
#ifndef XMALLOC_SIZE
#define XMALLOC_SIZE					40*1024
#endif

/** default stack size (in bytes) for threads */
#ifndef DEFAULT_STACKSIZE
#define DEFAULT_STACKSIZE				2000
#endif

/** stack size (in bytes) for scheduler
 * - ISRs and Scheduler are using the main stack (MSP)
 * - The array "char __schedulerStack__[SCHEDULER_STACKSIZE];" in scheduler.cpp is not used!
 * - So we must provide enough stack in the linker script (see "_Min_Stack_Size" in stm32_flash.ld)!
 */
#define SCHEDULER_STACKSIZE 			0

/** time interval between timer interrupts in microseconds - max. 798000us (if CPU_CLK==168MHz)!!! */
#ifndef PARAM_TIMER_INTERVAL
#define PARAM_TIMER_INTERVAL			10000
#endif

/*** time for time slice to swtich between threads with same priority ***/
#ifndef TIME_SLICE_FOR_SAME_PRIORITY
#define TIME_SLICE_FOR_SAME_PRIORITY	(100*MILLISECONDS)
#endif

/** default priority for newly created threads */
#ifndef DEFAULT_THREAD_PRIORITY
#define DEFAULT_THREAD_PRIORITY			100
#endif

/** user threads shall not have a priority higher than this */
#ifndef MAX_THREAD_PRIORITY
#define MAX_THREAD_PRIORITY				1000
#endif

/** high priority levels for priority ceiling  */
#ifndef NETWORKREADER_PRIORITY
#define NETWORKREADER_PRIORITY			(MAX_THREAD_PRIORITY + 2)
#endif
/** high priority levels for priority ceiling  */
#ifndef CEILING_PRIORITY
#define CEILING_PRIORITY				(NETWORKREADER_PRIORITY + 1)
#endif
/** using a network, the maximal number of nodes attached */
#ifndef MAX_NUMBER_OF_NODES
#define MAX_NUMBER_OF_NODES				10
#endif
/** if using network it may have a limit for pakets, eg udp 1500 */
#ifndef MAX_NETWORK_MESSAGE_LENGTH
#define MAX_NETWORK_MESSAGE_LENGTH		1300
#endif
/*** If using a network: Maximal number of subscribers per node */
#ifndef MAX_SUBSCRIBERS
#define MAX_SUBSCRIBERS					60
#endif

// Declare global variables with this to put them into the 64k-CoreCoupled Memory Block
// No DMA is possible. No initialization is possible,all gets set to zero.
#define PUT_INTO_CCRAM __attribute__ ((section (".bss_ccram")))

#ifdef __cplusplus
#ifndef NO_RODOS_NAMESPACE
}
#endif
#endif

#endif /* __PARAMS_H__ */
