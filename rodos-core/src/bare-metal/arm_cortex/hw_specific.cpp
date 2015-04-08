

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
* @file hw_specific.cc
* @date 2008/04/23 7:33
* @port 2010/02/18 19:14
* @author Miroslaw Sulejczak
*
* Copyright 2010 DLR
*
* @brief all hardware specific stuff I have no better place for ...
*
*/
#include "hw/hw_types.h"
#include "hw/hw_memmap.h"
#include "watchdog.h"

#include "params.h"
#include "hwinit.h"
#include "hw_specific.h"  // SergioM
#include "portmacro.h"
#include "src/uart.h"
#include "src/sysctl.h"
#include "src/systick.h"
#include "src/interrupt.h"
#include "src/uart.h"
#include "src/gpio.h"
#include "src/cpu.h"
#include "utils/uartstdio.h"
#include <sys/stat.h>
#include <errno.h>
//#include "yprintf.h"




/* Constants required to set up the initial stack. */
#define INITIAL_XPSR			( 0x01000000 )



void prvSetMSP( unsigned long ulValue ) __attribute__ (( naked ));
void prvSetPSP( unsigned long ulValue ) __attribute__ (( naked ));
volatile long taskRunning = 0;



extern "C" unsigned long long nanoTime;
extern "C" unsigned long long microTimeInterval;

unsigned long long hwGetNanoseconds()
{
	return nanoTime;
}
void hwInitTime()
{
	nanoTime=0;
}
unsigned long long hwGetAbsoluteNanoseconds(void) 
{
	return hwGetNanoseconds();// + timeAtStartup;
}


void prvSetPSP( unsigned long ulValue )
{
	__asm volatile( "msr psp, r0" );
	__asm volatile( "bx lr" );
}
/*-----------------------------------------------------------*/

void prvSetMSP( unsigned long ulValue )
{
	__asm volatile( "msr msp, r0" );
	__asm volatile( "bx lr" );
}
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void prvSetupTimerInterrupt( void )
{
	/* Configure SysTick to interrupt at the requested rate. */
    //
    // Configure SysTick for a 100Hz interrupt.  
    // wants a 10 ms tick.
    //
	SysTickPeriodSet(SysCtlClockGet() / 100);
	SysTickIntEnable();
	SysTickEnable();
	
}

void startIdleThread()
{
   	/* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
	*(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;
	*(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI;


	/* Start the first task. */
	prvSetPSP( 0 );

	*(portNVIC_INT_CTRL) |= portNVIC_PENDSVSET;

	/* Enable interrupts */
	portENABLE_INTERRUPTS();
}

void hwInit()
{
	taskRunning = 0;
	*(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;
	hardwareInit();
	lwip_init();

    return;
    
}



/*-----------------------------------------------------------*/


/**
*  abort execution
*/
void hwResetAndReboot() {
  UARTprintf("hw_resetAndReboot() -> exit");
  //abort();
}

/**
  *  global interrupt disable
  */
void hwDisableInterrupts(){
    CPUcpsid();
}

/**
  *  global interrupt enable
  */
void hwEnableInterrupts(){
    CPUcpsie();
}

/*
 * Initialize and start HW-watchdog
 * - 32 Bit counter running at systemclock (50MHz)
 * - watchdog resets the system on the second timeout!
 * -> min time: (1/50000000) * 2 = 40 ns
 * -> max time: (2^32/50000000) * 2 = 171 798 ms
 *
 */
static volatile unsigned long glWDLoad = 0;

void hwInitWatchdog(long intervalMilliseconds) {
	if (SysCtlPeripheralPresent(SYSCTL_PERIPH_WDOG)){
		SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG);

		WatchdogUnlock(WATCHDOG_BASE);			// unlock WD register

		WatchdogResetEnable(WATCHDOG_BASE);		// enable reset capability on second timeout
		/* watchdog resets the system on the second timeout!
		 * -> so we have to divide the time by 2
		 * -> on the first time-out only an interrupt is generated
		 */
		glWDLoad = (SysCtlClockGet()/1000) * (intervalMilliseconds/2);
		WatchdogReloadSet(WATCHDOG_BASE, glWDLoad);
		WatchdogStallEnable(WATCHDOG_BASE); 	// stops the watchdog during debug break
		WatchdogIntUnregister(WATCHDOG_BASE); 	// mask interrupt -> interrupts are not seen and handled by processor
		WatchdogEnable(WATCHDOG_BASE);

		WatchdogLock(WATCHDOG_BASE);			// lock WD register
	}
}

/*
 * trigger HW-watchdog -> must be done periodically with less than "intervalMilliseconds" ms
 */
void hwTriggerWatchdog() {
	//if (WatchdogIntStatus(WATCHDOG_BASE,false)){ // check raw interrupt status
	//	xprintf("WD interrupt\n");
		WatchdogIntClear(WATCHDOG_BASE);
	//}

	WatchdogUnlock(WATCHDOG_BASE);			// unlock WD register
	WatchdogReloadSet(WATCHDOG_BASE,glWDLoad);
	WatchdogLock(WATCHDOG_BASE);			// lock WD register
}

/**
* create context on stack and return a pointer to it
*/
long* hwInitContext(long* stack, void* object) {

	stack--; // to maintain the initial stack pointer double word aligned
	         // we have to decrement the stack pointer by an even number (FA-2012.02

	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */
	*stack = INITIAL_XPSR;	/* xPSR */
	stack--;
	*stack = (long)(threadStartupWrapper);	/* PC */
	stack--;
	*stack = 0;	/* LR */
	stack -= 5;	/* R12, R3, R2 and R1. */
	*stack = (long) object; 	/* R0 */
	stack -= 8;	/* R11, R10, R9, R8, R7, R6, R5 and R4. */

	return stack;
}

/** the timer interval */
long long Timer::microsecondsInterval = PARAM_TIMER_INTERVAL;

void Timer::init() {

	*(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI;
	SysTickPeriodSet((SysCtlClockGet() * Timer::microsecondsInterval) / 1000000);
	microTimeInterval=Timer::microsecondsInterval;
        SysTickIntEnable();
}

/**
* start timer 
*/
void Timer::start() 
{
	SysTickPeriodSet((SysCtlClockGet() * Timer::microsecondsInterval) / 1000000);
	microTimeInterval=Timer::microsecondsInterval;
	SysTickEnable();
}

/**
* stop timer 
*/
void Timer::stop() 
{
	SysTickDisable();
}

/**
* set timer interval 
*/
void Timer::setInterval(const long long microsecondsInterval) {
  Timer::microsecondsInterval = microsecondsInterval;
}

