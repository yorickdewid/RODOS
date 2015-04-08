/**
 * @file context_switch.S
 * @date 2012/04/16
 * @author Michael Ruffer
 *
 * Copyright 2012 University Wuerzburg
 *
 * assembly function for context switching ...
 *
 */
/* ------------------------------------------------------------------------- */

//	#include "core_sc0_140.h"
	
	.file "context_switch.S"
	.section ".text"

.align 4
SCHEDULER_WRAPPER_ADDR: .word schedulerWrapper

/* ------------------------------------------------------------------------- */
/**
 * @brief	Restore context of thread
 *
 * C equivalent:
 *     void __asmSwitchToContext(long* context)
 * 
 * r12 contains the address of the saved context
 */
__asmSwitchToContext:
	.global __asmSwitchToContext
	.func __asmSwitchToContext
	.type __asmSwitchToContext,"function"

	mov		sp, r12				// load pointer to thread context
	
	/* first load r0-r7 of thread context */
	popm 	r0-r7				// load R0-R7 from stack

	/* check which mode the cpu is in -> interrupt or supervisor?
	 * -> depending on the mode the cpu is in, we have to restores
	 *    r8-r12,lr,pc,sr in different ways
	 */
	mfsr   	r8,0				// get status register
	movh 	r9,HI(0x01800000)	// bitmask for mode bits M3&M2
	and		r8,r9				// a logical "AND" of status register and bitmask sets all bits to zero except M3&M2
	cp.w	r8,0				// if M3 and M2 are 0 the cpu is in supervisor or app mode
	breq	no_interrupt		// if M3 and M2 are 0 branch to no_interrupt
	rete						// cpu is in interrupt mode, so we must return with "rete" instruction
								// "rete" restores r8-r12,lr,pc,sr automatically from stack
no_interrupt:
	ld.w	r8,sp++				// load status register from stack
	mtsr   	0,r8				// move r0 to status register
								// -> interrupts will be enabled when loading r8 to status register
								//    -> a timer interrupt occurs immetiatly due to calling
								//       Timer::start() before entering __asmSwitchToContext
								//       -> no worries ... a repeated scheduler call is prevented in ISR
	popm 	r8-r12,lr,pc		// load r8-r12,lr,pc from stack
.endfunc

/* ------------------------------------------------------------------------- */
/**
 * @brief	Switch to next thread ready to run
 * 
 * C equivalent:
 *     void __asmSaveContextAndCallScheduler()
 * 
 * Called by yield().
 */
__asmSaveContextAndCallScheduler:
	.global __asmSaveContextAndCallScheduler
	.func __asmSaveContextAndCallScheduler
	.type __asmSaveContextAndCallScheduler,"function"
	
	ssrf	16				// disable interrupts - Set Status Register Flag "Global Interrupt Mask" */

	/* save context on stack */
	pushm 	r8-r12,lr		// save R8-R12 & LR on stack
	st.w	--sp,lr			// save LR again on stack -> LR becomes the new PC when resuming the thread
	mfsr   	r8,0			// copy status register to r8
	cbr		r8,16			// Clear Status Register Flag "Global Interrupt Mask" -> interrupts are enabled after resume
	st.w	--sp,r8			// save status register on stack
	pushm 	r0-r7			// save R0-R7 on stack

	/* prepare call to scheduler */
	mov		r12, sp							// pointer to context as parameter to schedulerWrapper
	mov		sp, LO(__schedulerStackFrame__) // load address of scheduler stack
	orh		sp, HI(__schedulerStackFrame__) // load address of scheduler stack
	ld.w	sp, sp							// load scheduler stack
	ld.w	pc, SCHEDULER_WRAPPER_ADDR		// branch to  scheduler
.endfunc
