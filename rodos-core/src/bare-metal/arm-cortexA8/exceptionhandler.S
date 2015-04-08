@******************************************************************************
@
@ exceptionhandler.S - Definitions of exception handlers
@
@******************************************************************************
@
@ Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
@
@
@  Redistribution and use in source and binary forms, with or without
@  modification, are permitted provided that the following conditions
@  are met:
@
@    Redistributions of source code must retain the above copyright
@    notice, this list of conditions and the following disclaimer.
@
@    Redistributions in binary form must reproduce the above copyright
@    notice, this list of conditions and the following disclaimer in the
@    documentation and/or other materials provided with the
@    distribution.
@
@    Neither the name of Texas Instruments Incorporated nor the names of
@    its contributors may be used to endorse or promote products derived
@    from this software without specific prior written permission.
@
@  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
@  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
@  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
@  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
@  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
@  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
@  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
@  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
@  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
@  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
@  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
@
@******************************************************************************
@
@ Share header file with assembly source code
@
#include "hw_intc.h"
#include "soc_AM335x.h"

@************************** Global symbols ************************************
        .global IRQHandler
        .global FIQHandler
        .global AbortHandler
        .global SVCHandler
        .global UndefInstHandler
        .global CPUAbortHandler
        .global fnRAMVectors       
        
        .equ ADDR_SIR_IRQ, SOC_AINTC_REGS + INTC_SIR_IRQ
        .equ ADDR_SIR_FIQ, SOC_AINTC_REGS + INTC_SIR_FIQ
        .equ ADDR_CONTROL, SOC_AINTC_REGS + INTC_CONTROL
        .equ ADDR_THRESHOLD, SOC_AINTC_REGS + INTC_THRESHOLD
        .equ ADDR_IRQ_PRIORITY, SOC_AINTC_REGS + INTC_IRQ_PRIORITY

        .equ MASK_ACTIVE_IRQ, INTC_SIR_IRQ_ACTIVEIRQ
        .equ MASK_ACTIVE_FIQ, INTC_SIR_FIQ_ACTIVEFIQ
        .equ NEWIRQAGR, INTC_CONTROL_NEWIRQAGR
        .equ NEWFIQAGR, INTC_CONTROL_NEWFIQAGR

        .equ MASK_SVC_NUM, 0xFF000000
        .equ MODE_SYS, 0x1F
        .equ MODE_IRQ, 0x12 
        .equ I_BIT, 0x80

@**************************** Text Section ************************************
        .text

@ This source file is assembled for ARM instructions
        .code 32
@******************************************************************************
@*                  Function Definition of SWI Handler
@******************************************************************************    
@
@ The SVC Handler switches to system mode if the SVC number is 458752. If the
@ SVC number is different, no mode switching will be done. No other SVC are 
@ handled here
@
SVCHandler:
        STMFD    r13!, {r0-r1, r14}       @ Save context in SVC stack
        SUB      r13, r13, #0x4           @ Adjust the stack pointer
        LDR      r0, [r14, #-4]           @ R0 points to SWI instruction
        BIC      r0, r0, #MASK_SVC_NUM    @ Get the SWI number
        CMP      r0, #458752
        MRSEQ    r1, spsr                 @ Copy SPSR  
        ORREQ    r1, r1, #0x1F            @ Change the mode to System
        MSREQ    spsr_cf, r1              @ Restore SPSR
        ADD      r13, r13, #0x4           @ Adjust the stack pointer
        LDMFD    r13!, {r0-r1, pc}^       @ Restore registers from IRQ stack

@******************************************************************************
@*                  Function Definition of IRQ Handler
@******************************************************************************    
@
@ The IRQ handler jumps to the ISR of highest priority pending IRQ.
@ This handler is a prioritized interrupt handler. The handler is a
@ re-entrant IRQ handler. So interrupt service routines are processed
@ in system mode to avoid lr_irq corruption
@
@ARM_IRQ:
	@ Adjust the return address
 IRQHandler:
	sub		lr, lr, #4
	push	{r0, r1}
  LDR      r1, =ADDR_SIR_IRQ        @ Get the Active IRQ
  LDR      r0, [r1]
	ldr		r1, =vectNum
	str		r0, [r1]
	ldr		r1, =511
	and		r0, r0, r1				@ Mask the interrupt ID
	cmp		r0, #68					@ Compare with EPIT1 interrupt ID
  MOV      r0, #NEWIRQAGR           @ To enable new IRQ Generation
  LDR      r1, =ADDR_CONTROL
  STReq    r0, [r1]                 @ if > 0 priority, acknowledge INTC
  DSB                               @ Make sure acknowledgement is completed
	pop/*eq*/	{r0, r1}
	beq		irq_context_switch		@ If it's EPIT1 then jump to irq_context_switch



	/*mrc     p15, 4, r0, c15, c0, 0  @ Read periph base address
	add		r0, r0, #0x100			@ CPU interface offset
	ldr		r0, [r0, #(4*3)]		@ Offset for the IAR register. Is the 4th register.*/
  
	/*@mrc		p15, 0, r1, c0, c0, 5
	and   	r1, r1, #3
    cmp		r0, r1					@ Compare for SGI interrupt ID
    popeq	{r0, r1}
	beq		irq_context_switch*/

//	popne	{r0, r1}
	

/*IRQHandler:
        SUB      r14, r14, #4             @ Apply lr correction
*/



        STMFD    r13!, {r0-r12, r14}  @ Save context
        MRS      r12, spsr                @ Copy spsr
        STMFD    r13!, {r12}              @ Save spsr

     @   LDR      r0, =ADDR_THRESHOLD      @ Get the IRQ Threshold
     @   LDR      r1, [r0, #0]
     @   STMFD    r13!, {r1}               @ Save the threshold value

     @   LDR      r2, =ADDR_IRQ_PRIORITY   @ Get the active IRQ priority
     @   LDR      r3, [r2, #0]
 @       STR      r3, [r0, #0]             @ Set the priority as threshold

        LDR      r1, =ADDR_SIR_IRQ        @ Get the Active IRQ
        LDR      r2, [r1]


        AND      r2, r2, #MASK_ACTIVE_IRQ @ Mask the Active IRQ number
	
        
        
      @  MOV      r0, #NEWIRQAGR           @ To enable new IRQ Generation
      @  LDR      r1, =ADDR_CONTROL
        
        


      @  CMP      r3, #0                   @ Check if non-maskable priority 0
    @    STRNE    r0, [r1]                 @ if > 0 priority, acknowledge INTC
      @  DSB                               @ Make sure acknowledgement is completed
        
        @
        @ Enable IRQ and switch to system mode. But IRQ shall be enabled
        @ only if priority level is > 0. Note that priority 0 is non maskable.
        @ Interrupt Service Routines will execute in System Mode.
        @
        MRS      r14, cpsr                @ Read cpsr
        ORR      r14, r14, #MODE_SYS
    @    BICNE    r14, r14, #I_BIT         @ Enable IRQ if priority > 0
        MSR      cpsr, r14
                             
       @ New from Tobi        
      @  cmp		r2, #68					@ Compare with EPIT1 interrupt ID
	     @ beq		irq_context_switch		@ If it's EPIT1 then jump to irq_context_switch

        STMFD    r13!, {r14}              @ Save lr_usr

        LDR      r0, =fnRAMVectors        @ Load the base of the vector table
        ADD      r14, pc, #0              @ Save return address in LR
       LDR      pc, [r0, r2, lsl #2]     @ Jump to the ISR
 
        LDMFD    r13!, {r14}              @ Restore lr_usr

        @
        @ Disable IRQ and change back to IRQ mode
        @
        CPSID    i, #MODE_IRQ

        @LDR      r0, =ADDR_THRESHOLD      @ Get the IRQ Threshold

        @LDR      r1, [r0, #0]
        @CMP      r1, #0
        MOV    r2, #NEWIRQAGR           @ Enable new IRQ Generation
        LDR    r1, =ADDR_CONTROL
        STR    r2, [r1]
       	DSB
      @  LDMFD    r13!, {r1}
     @   STR      r1, [r0, #0]             @ Restore the threshold value
        LDMFD    r13!, {r3}
        MSR      spsr, r3                 @ Restore spsr

        LDMFD    r13!, {r0-r12, pc}^  @ Restore the context and return


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
	msr		cpsr, #MODE_SYS|I_BIT
	mov		r0, sp
	mov		r1, lr
	msr		cpsr, #MODE_IRQ|I_BIT
	stmia	lr!, {r0,r1}
	pop		{r0}		@ This contains the LR_IRQ, which is the PC for the thread
	stmia	lr!, {r0}

	@ Call the leave_irq to prepare for the context switch
	ldr		r12, =leave_irq
	blx		r12

	@ack interrupt
	MOV      r0, #NEWIRQAGR           @ To enable new IRQ Generation
    LDR      r1, =ADDR_CONTROL
    STR    r0, [r1]                 @ if > 0 priority, acknowledge INTC

	ldr		r12, =__asmSaveContext
	blx		r12


@******************************************************************************
@*                  Function Definition of FIQ Handler
@******************************************************************************    
@
@ FIQ is not supported for this SoC.
@
FIQHandler:
        SUBS     pc, r14, #0x4            @ Return to program before FIQ

@******************************************************************************
@*             Function Definition of Abort/Undef Handler
@******************************************************************************    
@
@ The Abort handler goes to the C handler of abort mode. Note that the undefined
@ instruction is not handled separately.
@ if nothing is done in the abort mode, the execution enters infinite loop.
@
AbortHandler:
UndefInstHandler:
@
@ Disable all the interrupts
@
    @    MRS     r0, cpsr                  @ Read from CPSR
    @    ORR     r0, r0, #0xC0             @ Clear the IRQ and FIQ bits    
    @    MSR     cpsr, r0                  @ Write to CPSR
    @    ADD     r14, pc, #0               @ Store the return address
    @    LDR     pc, =CPUAbortHandler      @ Go to C handler
@
@ Go to infinite loop if returned from C handler
@
loop0:
        B       loop0                      


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


    /* Switch to data section */
    .data
    /* Place to store whether we got a prefetch or data abort. */
    /* 0 = data abort */
    /* 1 = prefetch abort */
    /* 2 = undifined address */
    /* 3 = error */
.Labort_type:
    .word   0







@*****************************************************************************
@
@ End of the file
@
    .end
    
    
    
