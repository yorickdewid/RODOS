/*
 * This file contains source code copyrighted by David Welch distributed under the terms of the MIT license.
 */

/**
 * @file hw_specific.cpp
 * @date 2013/06/01
 * @author Johannes Freitag, Sergio Montenegro, David Welch
 *
 * Copyright 2013 DLR and University Wuerzburg
 */

#include <rodos.h>
#include <hal/hal_uart.h>

#include "params.h"
#include "hw_specific.h"
#include "bcm2835.h"

volatile long *contextT;

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/*********************************************************************************************/
/* CONTEXT SWITCH AND INTERRUPT HANDLING */
/*********************************************************************************************/

extern int64_t timeToTryAgainToSchedule;
extern bool isSchedulingEnabled;

#define CONTEXT_SAVE  	__asm volatile ( \
	/* push r0 to use the register*/ \
	"push {r0}            \n\t"  \
	/* set r0 to point to the task stack pointer */ \
	"stmdb sp, {sp}^      \n\t" /* ^ means get the system mode SP value */  \
	"sub sp, sp, #4       \n\t" \
	"pop {r0}             \n\t" \
	/* push the return address onto the task stack */ \
	"stmdb r0!, {lr}      \n\t" \
	/* now we have saved lr we can use it instead of r0 */ \
	"mov lr, r0           \n\t" \
	/* pop r0 so we can save it onto the task stack */ \
	"ldmia sp!, {r0}      \n\t" \
	/* push all the system mode registers onto the task stack */ \
	"stmdb lr,{r0-r14}^   \n\t" \
	"sub lr, lr, #60      \n\t" \
	/* push all the fpu mode registers onto the task stack */ \
	"vstmdb lr!,{s0-s31}   \n\t" \
	/* Push the SPSR onto the task stack. */ \
	"MRS R0, SPSR         \n\t" \
	"STMDB LR!, {R0}      \n\t" );

#define CONTEXT_LOAD  	__asm volatile ( \
	/* get the top of stack for the task */  \
	"ldr r0, =contextT     \n\t" \
	/* move the task stack pointer into the Link Register */  \
	"ldr lr, [r0]          \n\t" \
	/* Get the SPSR from the stack. */  \
	"LDMFD LR!, {R0}       \n\t" \
	"MSR SPSR_cxsf, R0     \n\t" \
	/* restore all fpu registers */  \
	"vldmia lr!, {s0-s31}   \n\t" \
	/* restore all system mode registers for the task */  \
	"ldmfd lr, {r0-r14}^   \n\t" \
	/* restore the return address */  \
	"ldr lr, [lr, #+60]    \n\t" \
	/* return to next instruction in new task and leave irq */  \
	"movs pc, lr           \n\t" );

extern "C" {

extern void UART0_IRQHandler();

/*
 * Handles the hardware interrupt after saving the context and determines who has raised the interrupt
 */
void handleInterrupt(long int* context) {
	contextT = context;

	//handles the timer interrupts
	if (GET32(CS) & (1 << COUNTER1)) {
		//calc next ticktime
		uint32_t nextTick = GET32(CLOL) + PARAM_TIMER_INTERVAL; //alle 10ms // hier muss  Timer::microsecondsInterval

		if (isSchedulingEnabled == true) {
			TTime timeNow = NOW();
			TimeEvent::propagate(timeNow);
			if (NOW()> timeToTryAgainToSchedule) {
				// call scheduler with top of task stack
					schedulerWrapper(context);
				}
			}
			//set next tick time
		PUT32(C1, nextTick);
		//Write a one to the relevant bit to clear the match detect status bit and the corresponding interrupt request line.
		//BCM2835 ARM Peripherals 172
		PUT32(CS, 0x2);
	}

	//handles the uart interrupt
	if (AUX_MU_IIR_RX_IRQ) {
		UART0_IRQHandler();
	}

}


/**
 * pushes the context to the old task stack - handles hardware interrupts - pops the context of the new task from the new task stack
 */
__attribute__ (( naked ))
void c_irq_handler(void) {

	//lr has to be adjusted in hardware irq
	__asm volatile ( "sub lr,lr,#4         \n\t" );

	// save old context
	CONTEXT_SAVE

	//call handler and sheduler
	__asm volatile (
			"mov r0, lr           \n\t"
			"bl handleInterrupt             \n\t" );

	// restore context of new task
	CONTEXT_LOAD

}

/**
 * pushes the context to the old task stack - schedules - pops the context of the new task from the new task stack
 *
 * contextSwitch is called from the ISR of the software interrupt
 * a software interrupt is raised in yield and in the irq_handler for preemtion
 * in swi the interrupts are disabled
 */
__attribute__ (( naked ))
void contextSwitch(void) {

	// save old context
	CONTEXT_SAVE

    // call scheduler with top of task stack in r0
	__asm volatile (
			"mov r0, lr            \n\t"
			"bl schedulerWrapper   \n\t");

	// restore context of new task
	CONTEXT_LOAD

}

void __asmSwitchToContext(long* context) {
	contextT = context;
}

void __asmSaveContextAndCallScheduler() {
	__asm volatile ("swi 0");
}

}			//end extern "C"

/**
 * create context on the stack and return a pointer to it
 *
 * this context has to look like it had been created by the contextSwitch
 */
long int* hwInitContext(long* stack, void* object) {

	uint32_t psp = (uint32_t) stack;

	//bottom of stack
	stack--;
	*stack = (int32_t) (threadStartupWrapper);	// lr in swi mode
	stack--;
	*stack = 0; 				// lr in sys mode
	stack--;
	*stack = psp; 				// sp
	stack--;
	*stack = 0; 				// r12
	stack--;
	*stack = 0; 				// r11
	stack--;
	*stack = 0; 				// r10
	stack--;
	*stack = 0; 				// r9
	stack--;
	*stack = 0; 				// r8
	stack--;
	*stack = 0; 				// r7
	stack--;
	*stack = 0; 				// r6
	stack--;
	*stack = 0; 				// r5
	stack--;
	*stack = 0; 				// r4
	stack--;
	*stack = 0; 				// r3
	stack--;
	*stack = 0; 				// r2
	stack--;
	*stack = 0; 				// r1
	stack--;
	*stack = (int32_t) object; 		// r0
	stack -= 32;				// FPU
	stack--;
	*stack = GETcpsr(); 	    		// SPSR
	*stack = *stack & ~0x80;    		//interrupts enabled
	//top of stack

	return stack;
}

/** switches to the context of the initial idle thread
 * called in main() -> Scheduler::idle();
 * basically the same as the "restore context of new task" part from c_swi_handler
 */
__attribute__ (( naked ))
void startIdleThread() {
	CONTEXT_LOAD
}




/*********************************************************************************************/
/* HARDWARE INITIALIZATION */
/*********************************************************************************************/

uint32_t nextfree;

//-------------------------------------------------------------------
uint32_t next_coarse_offset(uint32_t x) {
	uint32_t mask;

	mask = (~0) << (10 - 2);
	mask = ~mask;
	while (x & mask)
		x++;
	return (x);
}
//-------------------------------------------------------------------
uint32_t add_one(uint32_t add, uint32_t flags) {
	uint32_t ra;
	uint32_t rb;
	uint32_t rc;

	//bits 31:20 index into the top level table
	ra = add >> 20;
	rc = MMUTABLEBASE + (ra << 2);
	rb = GET32(rc);
	if (rb) {
		xprintf("Address %08X already allocated\n", add);
		return (1);
	}
	add = ra << 20;

	rb = next_coarse_offset(nextfree);
	rc = rb + COARSE_TABLE_WORDS;
	if (rc >= MMUTABLESIZE) {
		xprintf("Not enough room\n");
		return (1);
	}
	nextfree = rc;
	//use coarse page table pointer on top level table
	PUT32(MMUTABLEBASE + (ra << 2), (MMUTABLEBASE + (rb << 2)) | 0x00000001);
	//fill in the coarse page table. with small entries
	for (ra = 0; ra < COARSE_TABLE_WORDS; ra++) {
		PUT32(MMUTABLEBASE + (rb << 2) + (ra << 2), (add + (ra << 12)) | 0x00000032 | flags);
	}
	return (0);
}

/**
 * indicates that a task is running
 */
volatile long taskRunning = 0;

HAL_UART uart_stdout(UART_DEBUG);

/** function to do hardware initialization
 * -> hwInit() is called in main() (src/independent/main.cpp)
 */
void hwInit() {

	taskRunning = 0;

	// Init UART
	uart_stdout.init();

	/****** build page tables *****/

	//from  0x200000000 i/o
	//until 0x100000000 pagetable
	//until 0x0FF000000 program

	//Initialize level 1 page table
	for (nextfree = 0; nextfree < TOP_LEVEL_WORDS; nextfree++)
		PUT32(MMUTABLEBASE + (nextfree << 2), 0);

	//level 2 - ram used by the stack and the program - cached
	for (int32_t addr = 0; addr < 0x0FF00000; addr+=0x00100000) {
		add_one(addr, 0x0000 | 8 | 4);
	}

	//level 2 - mMemory mapped I/O used by the uart, etc, not cached
	for (int32_t addr = 0x20000000; addr < 0x20900000; addr+=0x00100000) {
		add_one(addr, 0);
	}

	//activate mmu and l1 cache
	start_mmu(MMUTABLEBASE, 0x00800001);
	start_l1cache();

}


/**
 *  abort execution
 */
void hwResetAndReboot() {
	xprintf("hw_resetAndReboot() not implemented yet\n");
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


/**
 * In case a network is avialable, the corresponding node nr.
 */
/*
long myNodeNr = -1;

long getNodeNumber() {
	return myNodeNr;
}
*/

#ifndef NO_RODOS_NAMESPACE
}
#endif

