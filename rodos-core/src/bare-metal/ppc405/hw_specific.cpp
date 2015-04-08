/**
 * @file hw_specific.cpp
 * @author Lutz Dittrich, Andre Bartke <dev@bartke.cc>
 *         Tobias Mikschl <tobias.mikschl@uni-wuerzburg.de>
 *
 * @brief RODOS intefaces, hardware specifics and context initialization.
 */

#include <rodos.h>
#include <cache.h>
#include <registers.h>
#include "hw_specific.h"
#include "hw_stdlib.h"
#include <addresses.h>
#include <bsp.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

volatile long taskRunning = 0;

/* Call to _boot to ensure linkage of head.o */
/*         --- do not remove ---         --- */
extern "C" void _boot();
void call_boot() {
  _boot();
}

void hwINTCEnable() {
  
  *(INTC0_BASEADDR + intc_ierOffset)|= (1<<INTC_UART0);
  #ifdef USE_DEV_BOARD_BIROS
  *(INTC0_BASEADDR + intc_ierOffset)|= (1<<INTC_TC1);
  *(INTC0_BASEADDR + intc_ierOffset)|= (1<<INTC_TC2);
  #endif

  *(INTC0_BASEADDR + intc_merOffset) = 0x03;
}

void hwInit() {
  asm volatile (
    "li 0, 0\n"
    "mtmsr 0\n"
    : // no outputs
    : // no inputs
    : "0"
  ); // disable everything

  // enable caches
  icache_enable(0x80000000);
  dcache_enable(0x80000000);
	hwINTCEnable();
}

long* hwInitContext(long* stack, void* object) {
  long* tos = stack;

  tos = (long*)((unsigned int)tos & ~(16 / sizeof(tos) - 1));
  // 16-bytes alignment

  tos -= 16 / sizeof(tos);

  *(tos + 1) = (long)threadStartupWrapper; /* PC */

  stack = tos;

  tos -= 160 / sizeof(tos); /* stack frame with 40 elements */
  *(tos + 0) = (long)stack; /* thread initial stack pointer */
  *(tos + 3) = (long)stack; /* GPR1: stack pointer */
  *(tos + 5) = (long)object; /* GPR3: pointer to thread data */

  *(tos + 34) = 0; /* CR */
  *(tos + 35) = 0; /* XER */
  *(tos + 36) = 0; /* LR */
  *(tos + 37) = 0; /* CTR */

  // This is not necessary, the object pointer is only used once in in the
  // callback function so it doesn't need to be preserved. -AndreB
  //*(tos + 38) = (long)object; /* pointer to thread data */

  return tos;
}

extern "C" {
  int fflush(FILE *) {
    return 0;
  }

  void abort(void) {}
}

void hwResetAndReboot() {}

void sp_partition_yield() {}

void FFLUSH(void) {}

void startIdleThread() {}

// watchdog
static int64_t timeOfLastTriggerWatchdog = 0;

void hwInitWatchdog(long intervalMilliseconds) {
  /* Setup Watchdog:
   * -TCR_WP29    watchdog period 2^29 clocks
   * -TCR_WRC     watchdog chip reset
   * -TCR_WIE     watchdog interrupt enable
   *
   * Interrupt vector: 0x1020
   */

  // timer control register (TCR)
  asm volatile (
    "mftcr  0\n"
    "or     0, 0, %0\n"
    "mttcr  0\n"
    : // no outputs
    : "r" (TCR_WP29|TCR_WRC|TCR_WIE)
    : "0"
  );
  
}

void hwTriggerWatchdog() {
  timeOfLastTriggerWatchdog = NOW();

  // Clear TSR[WIS]
  // TSR[ENW] is never cleared, causing a watchdog timeout to force a reset
  asm volatile (
    "mttsr  %0\n"
    : // no outputs
    : "r" (TSR_WIS)
    : "0"
  );
}


/**
  *  global interrupt disable
  */
void hwDisableInterrupts(){
    asm volatile("wrteei 0" : : : "memory");
}

/**
  *  global interrupt enable
  */
void hwEnableInterrupts(){
    asm volatile("wrteei 1" : : : "memory");
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

