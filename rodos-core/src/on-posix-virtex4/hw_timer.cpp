

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
* @file hw_timer.cc
* @date 2008/04/23 16:36
* @author Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief fixed interval timer
*
* class for fixed-interval timer  for Linux as guest os
* uses the virtual timer (counts only when process is executed)
*/

#include <xmk.h>
#include <sys/intr.h>
#include <stdlib.h>

#include <signal.h>

#include "hw_specific.h"
#include <xtime_l.h>
#include <xexception_l.h>

// magic number of interrupts
#include <xparameters.h>
// XTImer
#include <xtmrctr_l.h>

#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

extern volatile long taskRunning;

/**
* Size of stack required by signal handler.
*/
#define SIGNAL_HANDLER_STACK_SIZE SIGSTKSZ

/** the timer interval */
long long Timer::microsecondsInterval = 10000;


/**
* The instruction pointer during the last interruption.
*/
volatile long __interruptedInstructionPointer__ = 0;


/**
* the signal handler for SIGVTALRM (timer signal)
*/
void timerSignalHandler(void* ignored) {

    volatile unsigned int val;  

    /* stop and reset timer and interrupt flag */
    XTmrCtr_SetControlStatusReg(XTIMER_BASE,0,2);

    /* time events to call? */
    TTime timeNow = NOW();  // comment this out to improve performance, but: no time events any more
    TimeEvent::propagate(timeNow); // comment this out to improve performance, but: no time events any more

    /* set the timer interrupt interval by configuring the load register */
    XTmrCtr_SetLoadReg(XTIMER_BASE,0,SYSTEM_CLOCK_IN_MHZ*(unsigned int)Timer::getInterval);
    XTmrCtr_LoadTimerCounterReg(XTIMER_BASE,0);

    /* clear load bit before starting the timer */
    val=XTmrCtr_GetControlStatusReg(XTIMER_BASE,0);
    val&=0xffffffdf;
    XTmrCtr_SetControlStatusReg(XTIMER_BASE,0,val);

    /* enable interrupt and counting */
    XTmrCtr_EnableIntr(XTIMER_BASE,0);
    XTmrCtr_Enable(XTIMER_BASE,0);
   
}

/**
* initialize the timer and signal handler 
*/
void Timer::init() {

    volatile unsigned int val;

    /* intialize the counter control/status register of timer 0:
       disable counting interrupts, count downwards, generate,
       reset interrupt flag */
    XTmrCtr_SetControlStatusReg(XTIMER_BASE,0,2);

    /* register the timer signal handler as generic interrupt handler
       for all interrupts of the interrupt controller 0 */
    register_int_handler(XPAR_XPS_INTC_0_XPS_TIMER_0_INTERRUPT_INTR, timerSignalHandler,NULL);

    /* enable the interrupt handler for interrupt controller 0 */
    enable_interrupt(XPAR_XPS_INTC_0_DEVICE_ID);

    /* set the timer interrupt interval by configuring the load register 
       to the init value and set the load bit to load the timer */
    XTmrCtr_SetLoadReg(XTIMER_BASE,0,SYSTEM_CLOCK_IN_MHZ*(unsigned int)Timer::getInterval);
    XTmrCtr_LoadTimerCounterReg(XTIMER_BASE,0);

    /* clear load bit before starting the timer */
    val=XTmrCtr_GetControlStatusReg(XTIMER_BASE,0);
    val&=0xffffffdf;
    XTmrCtr_SetControlStatusReg(XTIMER_BASE,0,val);

}

/**
 * Timer::stop() and Timer::start() are ignored.  The low-level
 * implementation does not require interrupts to be disabled manually.
 * The low-level code checks for supervisor/user mode and only
 * preempts if we're running in user mode, so the scheduler is safe.
 * Also, the stack switching code itsef is properly protected against
 * any interrupts.
 *
 * There are UART interrupts, too, so switching off the timer is no
 * guarantee that no interrupts come in.
 *
 * So there is no reason to switch off the timer.
 */
void Timer::start() {

  /* enable interrupt and counting */
  XTmrCtr_EnableIntr(XTIMER_BASE,0);
  XTmrCtr_Enable(XTIMER_BASE,0);

}

void Timer::stop() {

  XTmrCtr_DisableIntr(XTIMER_BASE,0);
  XTmrCtr_Disable(XTIMER_BASE,0);

}

/**
* set timer interval 
*/
void Timer::setInterval(const long long microsecondsInterval) {
  Timer::microsecondsInterval = microsecondsInterval;
}

/**
 * get timer interval
 */

long long Timer::getInterval(void) {
    return Timer::microsecondsInterval;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

