/* !
 * @file startup_secondary.s
 * @brief this file contains startup code with support for multi-core startup
 */

 #include "asm_defines.h"

	.global secondary_hwInit

	.code 32
    .section ".startup.secondary", "ax"

    .global _start_secondary
    .func _start_secondary

_start_secondary:
	mov 	r10, r0				@ save r0 for cores 1-3, r0 arg field passed by ROM
								@ r0 is a function pointer for a function to be executed by secondary cpus
	@ Read CPU ID
    mrc     p15,0,r0,c0,c0,5	@ Read multiprocessor affinity register
    and     r0, r0, #3			@ Mask off, leaving CPU ID field
    mov		r9, r0


init_stacks:
	/********************************************************************
	 * 1-Initialize processor mode stacks and registers.
	 */
	@ Top of stack with 0xDEADBEEF
    ldr		r0, =TOP_STACK_MARKER
    mov		r1, #1024
    mul		r2, r9, r1

    @ FIQ mode
    msr     CPSR_c, #(MODE_FIQ | IRQ_DISABLE | FIQ_DISABLE)
    ldr     r3, =__fiq_stack_top__
    sub		sp, r3, r2
    push	{r0}

    @ IRQ mode
    msr     CPSR_c, #(MODE_IRQ | IRQ_DISABLE | FIQ_DISABLE)
    ldr     r3, =__irq_stack_top__
    sub		sp, r3, r2
    push	{r0}

    @ ABT (Abort) mode
    msr     CPSR_c, #(MODE_ABT | IRQ_DISABLE | FIQ_DISABLE)
    ldr     r3, =__abt_stack_top__
    sub		sp, r3, r2
    push	{r0}

    @ UND (Undefined) mode
    msr     CPSR_c, #(MODE_UND | IRQ_DISABLE | FIQ_DISABLE)
    ldr     r3, =__und_stack_top__
    sub		sp, r3, r2
    push	{r0}

    @ SVC mode
    msr     CPSR_c, #(MODE_SVC | IRQ_DISABLE | FIQ_DISABLE)
    ldr     r3, =__svc_stack_top__
    sub		sp, r3, r2
    push	{r0}

    @ SYS mode
    msr     CPSR_c, #(MODE_SYS)
    ldr     r3, =__sys_stack_top__
    sub		sp, r3, r2
    push	{r0}


init_memory_sys:
	/********************************************************************
	 * 2-Initialize the memory system, including the MMU.
	 */
	ldr		r12, =init_memory_secondary
	blx		r12

	@ Enable FPU
	ldr		r12, =enable_neon_fpu
	blx		r12

	@ Enable SCU and leave SMP
	ldr		r12, =scu_enable
	blx		r12
	ldr		r12, =scu_leave_smp
	blx		r12

	/********************************************************************
	 * 3-Use the same address of the vector table for this instance
	 * of RODOS. So every interrupt will be treated as if for CPU0.
	 */
	ldr		r1, =__vectors_start
	@ Set Vector Base Address Register (VBAR) to point to this application's vector table
	MCR     p15, 0, r1, c12, c0, 0     @ Write Secure or Non-secure Vector Base Address
	MRC     p15, 0, r1, c1, c0, 0      @ Read SCTLR
	BIC     r1, #(1<<13)               @ Clear V-bit
	MCR     p15, 0, r1, c1, c0, 0      @ Write SCTLR


secondary_cpus_init:
	ldr		r12, =secondary_hwInit
	blx		r12

	blx		r10

	b		.

    .endfunc
.end
