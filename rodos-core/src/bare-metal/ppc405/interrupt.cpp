/**
 * @file interrupt.cpp
 * @author Tobias Mikschl <tobias.mikschl@uni-wuerzburg.de>
 *
 * @brief C Interrupt-Routines.
 */

#include <rodos.h>
#include <registers.h>
#include <addresses.h>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

extern "C"{
  void UART0_IRQHandler();
#ifdef USE_DEV_BOARD_BIROS
	void TC1_IRQHandler();
	void TC2_IRQHandler();
#endif
}



/**
 * Extrenal Interrupt.
 * Processes Interrupts of the external Interruptcontroller.
 */
extern "C" void interruptExternal() {
  //xprintf("I:%d",*(INTC0_BASEADDR +intc_ivrOffset));
  switch(*(INTC0_BASEADDR +intc_ivrOffset)) {
    case INTC_UART0:
      // valid Data in UART Rx Buffer
      UART0_IRQHandler(); 
     
      /* clear UART interrupt */
      *(INTC0_BASEADDR + intc_iarOffset)= (1<<INTC_UART0);
      break;
    #ifdef USE_DEV_BOARD_BIROS
    case INTC_TC1:
      TC1_IRQHandler();
      /* Clear TC1 Interrupt */
      *(INTC0_BASEADDR + intc_iarOffset)= (1<<INTC_TC1);
      break;
    case INTC_TC2:
      TC2_IRQHandler();
      /* Clear TC2 Interrupt */
      *(INTC0_BASEADDR + intc_iarOffset)= (1<<INTC_TC2);
      break;
    #endif
    default:
      /* non handled external Interrupt -> just clear */
      *(INTC0_BASEADDR + intc_iarOffset)= (1<<*(INTC0_BASEADDR +intc_ivrOffset));
      break;
  }

 
}



extern "C" void __asmCallScheduler();
extern long long timeToTryAgainToSchedule;
extern bool isSchedulingEnabled;

 
/** 
 * Programmable Timer Interrupt.
 * Calls Scheduler if scheduling is
 * enabled and its time to.
 */
extern "C" void interruptPit() {
  if(!isSchedulingEnabled) return;
  long long timeNow = NOW();
  TimeEvent::propagate(timeNow); 
  if(timeNow < timeToTryAgainToSchedule) return;
  __asmCallScheduler();
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

