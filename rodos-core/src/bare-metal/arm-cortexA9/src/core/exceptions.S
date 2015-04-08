/**********************************************************************************
 * Exception handlers
 */
#include "asm_defines.h"

    .code 32

  	.global context_to_be_saved
 	.global vectNum				@ Variable that contain the irq number. Used in _ISR_hanlder
 	.global leave_irq
 	.global __asmSaveContext
 	.global _ISR_handler
 	.global FIQ_hanlder

	.section ".exceptions", "ax"
/*************************************************
  void ARM_IRQ(void)
 ************************************************/
	.global ARM_IRQ
	.func ARM_IRQ
ARM_IRQ:
	@ Adjust the return address
	sub		lr, lr, #4

	push	{r0, r1}
	mrc     p15, 4, r0, c15, c0, 0  @ Read periph base address
	add		r0, r0, #0x100			@ CPU interface offset
	ldr		r0, [r0, #(4*3)]		@ Offset for the IAR register. Is the 4th register.
	ldr		r1, =vectNum
	str		r0, [r1]
	ldr		r1, =511
	and		r0, r0, r1				@ Mask the interrupt ID
	cmp		r0, #88					@ Compare with EPIT1 interrupt ID
	popeq	{r0, r1}
	beq		irq_context_switch		@ If it's EPIT1 then jump to irq_context_switch
	mrc		p15, 0, r1, c0, c0, 5
	and   	r1, r1, #3
    cmp		r0, r1					@ Compare for SGI interrupt ID
    popeq	{r0, r1}
	beq		irq_context_switch

	popne	{r0, r1}
	bne		interrupt_handler

	@ Read cpu ID and check for the

irq_context_switch:
@ Save context in the structure - only save if the IRQ number is 88
	push	{lr}		@ The pointer to the context will be in the lr_irq
	ldr		lr, =context_to_be_saved

	@ Save the SPSR
	push	{r0}
	mrs		r0, spsr
	stmia	lr!, {r0}
	pop		{r0}

	@ Save registers
	stmia	lr!, {r0-r12}

	@ Switch to SYS mode to get the SP_SYS ans the LR_SYS
	msr		cpsr, #(MODE_SYS | IRQ_DISABLE |FIQ_DISABLE)
	mov		r0, sp
	mov		r1, lr
	msr		cpsr, #(MODE_IRQ | IRQ_DISABLE | FIQ_DISABLE)
	stmia	lr!, {r0,r1}
	pop		{r0}		@ This contains the LR_IRQ, which is the PC for the thread
	stmia	lr!, {r0}

	@ Call the leave_irq to prepare for the context switch
	ldr		r12, =leave_irq
	blx		r12
	ldr		r12, =__asmSaveContext
	blx		r12

@ Execute as normal interrupt
interrupt_handler:
	@ Move to SYS mode to store the context
	@msr     CPSR_c, #(MODE_SYS | IRQ_DISABLE)
	@Store context on our own stack
	push	{r0-r12,lr}


@==== Not needed when not alowwing IRQ during IRQ =====
	@ Switch to IRQ to restore R0 and R1

	@msr     CPSR_c, #(MODE_IRQ | IRQ_DISABLE)

	@ Copy SP_IRQ and LR_IRQ (the return address) to R0-R1
	@mov 	r0, sp
	@mov		r1, lr
	@mrs		r2, spsr

	@ Move to SYS mode to store the context
	@msr     CPSR_c, #(MODE_SYS | IRQ_DISABLE)
	@push	{r0-r2}

	/*--------------------------------------------------------------------------*/

	@ Call the interrupt routine - For now it will always run in IRQ mode
	bl		_ISR_handler

	/*--------------------------------------------------------------------------*/

	@ Restore context from stack
	@pop		{r0-r2}

	@ Store the LR_IRQ and SP_IRQ
	@msr     CPSR_c, #(MODE_IRQ | IRQ_DISABLE)
	@mov		sp, r0
	@mov		lr, r1
	@msr		spsr_fsxc, r2

	@ Switch back to SYS mode
	@msr     CPSR_c, #(MODE_SYS | IRQ_DISABLE)
	pop		{r0-r12,lr}

	@ Return from IRQ mode
	@msr     CPSR_c, #(MODE_IRQ | IRQ_DISABLE)
	movs	pc, lr
	.endfunc


/*************************************************
  void ARM_FIQ(void)
 ************************************************/
	.global ARM_FIQ
	.func ARM_FIQ
ARM_FIQ:
	bl		FIQ_hanlder
	.endfunc


/*************************************************
  void ARM_SWI(void)
 ************************************************/
	.global ARM_SWI
	.func ARM_SWI
ARM_SWI:
1:
	b       1b
	.endfunc


/*************************************************
  void exception_handler(void)
 ************************************************/
    .global exception_handler
    .func exception_handler
exception_handler:
1:
	b       1b
    .endfunc


/*
 * A very simple implementation of the data abort handler.
 *
 * It just dumps the registers before this exception occured, to make the debugging easier.
 */
/*************************************************
  void common_abort_handler(void)
 ************************************************/
    .global  common_abort_handler
    .func  common_abort_handler
common_abort_handler:
	push    {r0-r12}	/* push the general registers to this exception's stack */
	mrs     r7, cpsr
	mrs     r6, spsr
	mov     r5, sp
	msr     cpsr, r6    /* go to the mode before this exception so we can get its sp and pc */
	dsb
	stmfd   r5!, {sp, lr} /* push sp/lr to this exception's stack */
	msr     cpsr, r7    /* switch back to abort mode */
	dsb
	mov     sp, r5      /* restore our stack pointer */
	push    {r6, lr}    /* r6 is exception's cpsr, lr is the exception's pc */

	/* save coprocessor abort info registers */
	mrc     p15, 0, r4, c5, c0, 0   /* read DFSR */
	mrc     p15, 0, r3, c6, c0, 0   /* read DFAR */
	mrc     p15, 0, r2, c5, c0, 1   /* read IFSR */
	mrc     p15, 0, r1, c6, c0, 2   /* read IFAR */
	push    {r1-r4}        /* save DFSR, DFAR, IFSR, IFAR to stack */

	ldr     r1,=.Labort_type
	ldr     r0,[r1]     /* put abort type in r0 */
	mov     r1, sp      /* put address to saved registers into r1 */

	bl      dump_regs   /* call a C function to print the register values */

	/* clean up */
	pop     {r0-r12}
	add     sp, sp, #16 /* adjust the sp pointer, just like 'pop {lr, r6}' */

    /* Loop forever. */
1:
	b       1b

	sub     pc, lr, #4 /* abort handler's standard return, although never reach here. */
    .endfunc


/*************************************************
  void data_abort_handler(void)
 ************************************************/
    .global data_abort_handler
    .func data_abort_handler
data_abort_handler:
	ldr		sp,=__abt_stack_top__		@ Loadd the stack pointer for Abort Mode CPU0
    push    {r0-r1}
    ldr     r1,=.Labort_type
    mov     r0,#0
    str     r0,[r1]
    pop     {r0-r1}
    b       common_abort_handler
    .endfunc


/*************************************************
  void prefetch_abort_handler(void)
 ************************************************/
    .global prefetch_abort_handler
    .func prefetch_abort_handler
prefetch_abort_handler:
	ldr		sp,=__abt_stack_top__
    push    {r0-r1}
    ldr     r1,=.Labort_type
    mov     r0,#1
    str     r0,[r1]
    pop     {r0-r1}
    b       common_abort_handler
    .endfunc


/*************************************************
  void undefined_addr_handler(void)
 ************************************************/
	.global undefined_addr_handler
	.func undefined_addr_handler
undefined_addr_handler:
	ldr		sp,=__abt_stack_top__
    push    {r0-r1}
    ldr     r1,=.Labort_type
    mov     r0,#2
    str     r0,[r1]
    pop     {r0-r1}
    b       common_abort_handler
	.endfunc


/*************************************************
  void prefetch_abort_handler(void)
 ************************************************/
    .global error_handler
    .func error_handler
error_handler:
	ldr		sp,=__abt_stack_top__
    push    {r0-r1}
    ldr     r1,=.Labort_type
    mov     r0,#3
    str     r0,[r1]
    pop     {r0-r1}
    b       common_abort_handler
    .endfunc

	.global asmDisabelIRQ
	.func asmDisabelIRQ
	.type asmDisabelIRQ,"function"
asmDisabelIRQ:
	msr		CPSR_c, #(MODE_SYS | IRQ_DISABLE | FIQ_DISABLE)
	bx lr
.endfunc

	.global asmEnabelIRQ
	.func asmEnabelIRQ
	.type asmEnabelIRQ,"function"
asmEnabelIRQ:
	msr		CPSR_c, #(MODE_SYS)
	bx lr
.endfunc

    /* Switch to data section */
    .data
    /* Place to store whether we got a prefetch or data abort. */
    /* 0 = data abort */
    /* 1 = prefetch abort */
    /* 2 = undifined address */
    /* 3 = error */
.Labort_type:
    .word   0


.end
