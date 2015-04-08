/**
 * @file asm.s
 * @date 2013/03/22 15:11
 * @author Tiago Duarte
 *
 * Assembly function for context switching.
 */


.file "asm.s"
.section ".text"

/* ------------------------------------------------------------------------- */
/**
 * @brief Restore context of thread
 *
 * C equivalent:
 *     void __asmSwitchToContext(long* context)
 *
 * r0 contains the context passed as argument. Called by activate().
 */
        .equ MODE_SYS, 0x1F
        .equ MODE_IRQ, 0x12 
        .equ IRQ_DISABLE, 0x80
        .equ FIQ_DISABLE, 0x40

	.global __asmSwitchToContext
	.func __asmSwitchToContext
	.type __asmSwitchToContext,"function"
__asmSwitchToContext:
  msr		CPSR_c, #(MODE_SYS | IRQ_DISABLE | FIQ_DISABLE)
		@ Save copy of the context in the stack of SVC
	ldr		r1, =context_pointer
	str		r0, [r1]

	@ Update CPSR with context - the interrupts will be enabled by the load of the context
	ldmfd	r0!, {r1}		@ Load the ctx->CPSR to r1
	add		r2,r0,#52       @skip r0-r12 and load  sp,lr
	ldmfd	r2!, {sp,lr}

	msr		CPSR_c, #(MODE_IRQ | IRQ_DISABLE | FIQ_DISABLE) @ Now go to IRQ mode load rest of registers, put status un SPSR and set PC and status atomically
	ldmfd	r2, {lr} @ Get the PC in LR_irq
	msr		SPSR_fsxc, r1		@ All flags updated

	@ Load registers with context
	ldmfd	r0, {r0-r12}		@ Load to r0-r12 the value of ctx->r0-r12

	subs	pc,lr,#0  @ -4 is already done

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

 	.global __schedulerStackFrame__
 	.global schedulerWrapper
 	.global error_handler


 	.global __asmSaveContext
	.global __asmSaveContextAndCallScheduler
	.func __asmSaveContextAndCallScheduler
	.type __asmSaveContextAndCallScheduler,"function"
__asmSaveContextAndCallScheduler:
  @ Disable interrutps.
	msr		CPSR, #(MODE_SYS | IRQ_DISABLE | FIQ_DISABLE)

	@ Save context in the structure
	push	{lr}
	ldr		lr, =context_to_be_saved
	@ Save the CPSR
	push	{r0}
	mrs		r0, cpsr
	bic		r0, r0, #(IRQ_DISABLE | FIQ_DISABLE)	@ Clear interrupts
	stmia	lr!, {r0}
	@ Save registers
	pop		{r0}
	stmia	lr!, {r0-r12}
	pop		{r0}		@ This contains the LR
	stmia	lr!, {sp}
	stmia	lr!, {r0}	@ Save LR
	stmia	lr!, {r0}	@ Save PC with LR


__asmSaveContext:

	@ At this point the interrupts (IRQ) are disabled
	@ Switch to SVC mode and get the context address of the context to store and desable interrupts
	@msr		CPSR_c, #(MODE_SVC | IRQ_DISABLE | FIQ_DISABLE)
	@pop		{r0}		@ Contain the address of the context
	msr		CPSR_c, #(MODE_SYS | IRQ_DISABLE | FIQ_DISABLE)
	@mov		lr, r0		@ copy of the address of the context
	ldr		lr, =context_pointer
	ldr		lr, [lr]

	@ Put the address of the structure which contain the context, in SP
	ldr		sp, =context_to_be_saved

	@ Store the CPSR
	pop		{r0}
	stmia	lr!, {r0}

	@ Store the registers
	pop		{r0-r12}
	stmia	lr!, {r0-r12}

	@ Store the SP, LR and PC
	pop		{r0, r1, r2}
	stmia	lr!, {r0, r1, r2}

	@ Load *ctx to R0
	sub		r0, lr, #(17*4)

	@ Enable IRQ's
	@msr		CPSR_c, #(MODE_SYS)

	@ Call schedulerWrapper
	ldr		sp,=__schedulerStackFrame__	/* load address of scheduler stack */
	ldr		sp, [sp]					/* scheduler stack */
	ldr		r12, =schedulerWrapper		/* branch to  scheduler */
	bx 		r12							/* return */
.endfunc

.end
