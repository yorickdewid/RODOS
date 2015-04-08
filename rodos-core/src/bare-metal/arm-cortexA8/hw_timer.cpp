/*-----------------------------------------------------------
  Class Timer - provides start() and stop() methods that are
  intended to disable interrupts over the time of a voluntary
  context switch. If this is handled otherwise it is perfectly
  fine to also leave these methods unimplemented.
-------------------------------------------------------------*/

/*
 * Includes
 */

// RODOS includes
#include "hw_specific.h"
#include "rodos.h"

#include "dmtimer.h"
#include "soc_AM335x.h"
#include "interrupt.h"
#include "beaglebone.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


#define TICKS_PER_USEC 24

/*
 * Methods
 */

uint32_t timerStartValue;

void Timer::init(){
	 DMTimer2ModuleClkConfig();

//  /* Load the load register with the reload count value */
    DMTimerReloadSet(SOC_DMTIMER_2_REGS, timerStartValue);
    DMTimerCounterSet(SOC_DMTIMER_2_REGS, timerStartValue);
    DMTimerModeConfigure(SOC_DMTIMER_2_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);
//    /* Set the priority */
     IntPrioritySet(SYS_INT_TINT2, 0, AINTC_HOSTINT_ROUTE_IRQ);
//    /* Enable the system interrupt */
     IntSystemEnable(SYS_INT_TINT2);
}

void Timer::start(){
	DMTimerIntEnable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);
	DMTimerEnable(SOC_DMTIMER_2_REGS);
}

void Timer::stop(){
	DMTimerIntDisable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);
    DMTimerDisable(SOC_DMTIMER_2_REGS);
}

void Timer::setInterval(const long long microsecondsInterval){
	timerStartValue = 0xFFFFFFFF - (microsecondsInterval*TICKS_PER_USEC);
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

