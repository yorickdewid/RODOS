/**
 * @file asm.s
 * @date 2008/04/23 8:51
 * @author Lutz Dittrich
 *
 * Copyright 2008 DLR
 *
 * assembly function for context switching ...
 *
 */
/* ------------------------------------------------------------------------- */

	#include "constants.h"
	
	.file "asm.s"
	.section ".text"

/* ------------------------------------------------------------------------- */
/**
 * @brief	Restore context of thread
 *
 * C equivalent:
 *     void __asmSwitchToContext(long* context)
 * 
 * r0 contains the address of the saved context
 */
__asmSwitchToContext:
	.global __asmSwitchToContext
	.func __asmSwitchToContext
	.type __asmSwitchToContext,"function"
	swi		0   				/* switch to supervisor mode */
	
	mov		lr, r0
	
	ldmfd	lr!, {r4}			/* load PSR from top of stack */
	msr		SPSR_cxsf, r4		/* prepare saved processor status register */
	
	ldmfd	lr, {r0-r14}^		/* restore all (user mode) registers */
	add		lr, lr, #60			/* deallocate context memory */
	
	/* Return to thread
	 * - restore CPSR from SPSR
	 * - change mode to thumb when nessecary
	 * - exit supervisor mode */
	ldmfd	lr, {pc}^
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
	
	swi		0								/* switch to supervisor mode, sp (r13) and lr (r14) banked */
	
	/* get user mode stack pointer */
	sub		sp, sp, #4						/* allocate space */
	stm		sp, {sp}^						/* save user mode stack pointer */
	ldmfd	sp!, {lr}						/* lr = stack pointer of user mode */

	/* Save return address to stack.
	 * 
	 * Originating address saved in lr_usr, will be saved in __asmSaveContext.
	 * Label 0 is set as return address => when the thread is restored it returns
	 * there and jumps from there back to the yield() call. */
	stmfd	sp!, {r0}						/* save r0 */
	ldr		r0, =0f
	stmfd	lr!, {r0}
	ldmfd	sp!, {r0}						/* restore r0 */
	
	/* Assumes:
	 * - lr = user mode stack pointer
	 * - return address saved as last item on the stack
	 * 
	 * Called by timerSignalHandler (fiq_handler) 
	 */
__asmSaveContext:
	.global __asmSaveContext
	
	/* save context on stack */
	sub		lr, lr, #60						/* allocate space for context */
	stm		lr, {r0-r14}^					/* save (user mode) registers on stack */
	mrs		r4, SPSR						/* copy "saved PSR" to r4 */
	stmfd	lr!, {r4}						/* save "saved PSR" on top of stack */

	/* prepare call to scheduler */
	mov		r0, lr							/* pointer to context as parameter to */
											/* schedulerWrapper */
	
	mov		r4,  #MODE_SYS | IRQ_DISABLE | FIQ_DISABLE	/* scheduler runs in system mode */
	msr		CPSR_cxsf, r4					/* no thumb */
	
	ldr		sp, =__schedulerStackFrame__	/* load address of scheduler stack */
	ldr		sp, [sp]						/* scheduler stack */
	ldr		pc, =schedulerWrapper			/* branch to  scheduler */
	
0:
	bx lr									/* return */
.endfunc
