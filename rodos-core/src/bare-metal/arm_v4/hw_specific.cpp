/**
* @file hw_specific.cc
* @date 2008/04/23 7:33
* @author Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief all hardware specific stuff I have no better place for ...
*
*/

#include <stdint.h>

#include "context.h"
#include "debug.h"
#include "hw_specific.h"
#include "hw_uart.h"

#include "params.h"
#include "scheduler.h"
#include "semaphore.h"
#include "thread.h"

#include "constants.h"
namespace RODOS {
volatile long taskRunning = 0;

/**
*  abort execution
*/
void hwResetAndReboot() {
  //abort();
}

/**
  *  global interrupt disable
  */
void hwDisableInterrupts(){
    #warning "irq disable not tested yet"
    __asm volatile ("mov r1, #0xC0 \n\t" // disable FIQ & IRQ
                      "DisInt: \n\t"
                      "mrs r0,CPSR \n\t"
                      "orr r0,r0,r1 \n\t"
                      "msr CPSR,r0 \n\t"
                      "mrs r0,CPSR \n\t"
                      "ands r0,r0,r1 \n\t"
                      "beq DisInt");
}

/**
  *  global interrupt enable
  */
void hwEnableInterrupts(){
    #warning "irq enable not tested yet"
    __asm volatile ("mrs r0,CPSR \n\t"
                      "bic r0,r0,#0xC0 \n\t" // enable FIQ & IRQ
                      "msr CPSR,r0");
}


/* !!! watchdog implementation not tested due to toolchain problems !!! */
/*
 * Initialize and start HW-watchdog
 * - 32 Bit counter running at 4MHz/4 (internal RC oscillator/4) -> there is a fixed prescaler of 4 on wdclk
 * -> min time: (1/1000000) = 1 us
 * -> max time: (2^32/1000000) = 4 294 967 ms
 */
//#define WDEN		0x00000001
//#define WDRESET		0x00000002
//#define WDTOF		0x00000004
//#define WDINT		0x00000008
//
//
//void hwInitWatchdog(long intervalMilliseconds) {
//	WDCLKSEL = 0;						// select internal RC oscillator as clock source
//	WDTC = intervalMilliseconds*1000;	// once WDEN is set, the WDT will start after feeding
//	WDMOD = WDEN | WDRESET; 			// enable WD and reset on timeout
//
//	WDFEED = 0xAA;			// Feeding sequence
//	WDFEED = 0x55;
//}
//
///*
// * trigger HW-watchdog -> must be done periodically with less than "intervalMilliseconds" ms
// */
//void hwTriggerWatchdog() {
//	WDFEED = 0xAA;		/* Feeding sequence */
//	WDFEED = 0x55;
//}

/**
* create context on stack and return a pointer to it
*/
long* hwInitContext(long* stack, void* object) {
  long* tos = stack ;  /* top of stack */
  *tos-- = 0xdeaddead;	// debug
  TContext* ctx = (TContext*) (tos - sizeof(TContext)/sizeof(long));
  ctx->r0 = (long) object;
  ctx->PC = (long)(threadStartupWrapper);
  ctx->SP = (long)stack;
  asm volatile (
      "mrs  %0, CPSR     \n\t"
      : "=r"(ctx->CPSR)       // Output registers
  );
  return (long*) ctx;
}

void startIdleThread() { }

extern "C" int sched_yield();

void sp_partition_yield() { }
}
