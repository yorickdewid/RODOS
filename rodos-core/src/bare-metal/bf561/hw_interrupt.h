/*
 * hw_interrupt.h
 *
 *  Created on: 12.03.2014
 *      Author: wittkampm
 */

#ifndef HW_INTERRUPT_H_
#define HW_INTERRUPT_H_

#include "hw_config.h"
#include "string_pico.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * Makro to calculate valid interrupt channel numbers.
 */
#define PERIPHERAL_INTERRUPT_CHANNEL(CHANNEL)	((CHANNEL)&0x3f)

/**
 * runtime initialization of this hardware specific module
 */
void HW_InterruptInit();

/**
 * global disable of Interrupts
 */
void HW_disableInterrupts();
/**
 * global enable of Interrupts
 */
void HW_enableInterrupts();

/**
 * Mask (disable) the Interrupt of given ID.
 * @param id Interrupt ID
 */
void HW_maskInterrupts(unsigned int id);
/**
 * Unmask (enable) the Interrupt of given ID.
 * @param id Interrupt ID
 */
void HW_unmaskInterrupts (unsigned int id);
/**
 * Hook a service routine to a given Interrupt ID.
 * @param index Interrupt ID
 * @param interruptHandler Service Routine for this Interrupt ID
 */
void HW_setInterruptHandler(unsigned int index, isr_callback_no_arg*);
/**
 * @param peripheral_interrupt_channel
 * @param interrupt_vector_group defines the interrupt vector for the processor hierarchy. The value should be between 7 (highest) and 13 (lowest) priority.
 * @param wakeup if set to true the interrupt will wakeup the processor from idle or sleep states
 * @param isNoArg sets whether the callback is using arguments or not
 * @param callback the application service routine
 * @param a optional argument to the service routine
 * @return 0 in case of success
 */
u32 HW_setInterruptHandlerWithArgs(u32 peripheral_interrupt_channel, u8 interrupt_vector_group, bool wakeup, bool isNoArg, isr_callback* callback, void* arg);

/**
 * Get number of unhandled interrupts.
 */
unsigned long HW_getUnhandledInterruptsCounter();


void system_enter_critical_region(void *pArg);
void system_exit_critical_region(void *pArg);
bool isInterrupt();
bool isCritRegion();

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* HW_INTERRUPT_H_ */
