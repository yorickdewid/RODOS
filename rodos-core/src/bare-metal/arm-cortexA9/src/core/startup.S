/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES@
 * LOSS OF USE, DATA, OR PROFITS@ OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
  
/* !
 * @file startup.s
 * @brief this file contains startup code
 */

/*
 * Booting a bare-metal system
 * 1-In a multi-processor system, put non-primary processors to sleep
 * 2-Initialize exception vectors....done Here
 * 3-Initialize the memory system, including the MMU.....DOne Here
 * 4-Initialize processor mode stacks and registers....... "
 * 5-Initialize any critical I/O devices.
 * 6-Perform any necessary initialization of NEON or VFP....Done Here
 * 7-Enable interrupts.
 * 8-Change processor mode or state............Done Here
 * 9-Handle any set-up required for the Secure world.
 * 10-Call the main() application....Done Here
 */

#include "asm_defines.h"

	.global main				@ This is entry point. For rodos it will be main defined is /independent/main.cpp
	.global constructors_init
	.global secondary_hwInit

	.code 32
    .section ".startup", "ax"

    .global _start
    .func _start

_start:
	/********************************************************************
	 * 1-In a multi-processor system, put non-primary processors to sleep
	 * Check the core number and if 0 execute the initialization code
	 * If not put it to sleep
	 */
	@ Read CPU ID
    mrc     p15,0,r0,c0,c0,5	@ Read multiprocessor affinity register
    and     r0, r0, #3			@ Mask off, leaving CPU ID field
    mov		r9, r0


init_stacks:
	/********************************************************************
	 * 4-Initialize processor mode stacks and registers.
	 */
	ldr		r1, =__stack_low
    ldr		r2, =__stack_high
    ldr		r3, =STACK_FILL
stack_fill:
    stmia	r1!,{r3}
    cmp     r1,r2
    blt     stack_fill

	@ Top of stack with 0xDEADBEEF
    ldr		r0, =TOP_STACK_MARKER

    @ FIQ mode
    msr     CPSR_c, #(MODE_FIQ | IRQ_DISABLE | FIQ_DISABLE)
    ldr     sp, =__fiq_stack_top__
    push	{r0}

    @ IRQ mode
    msr     CPSR_c, #(MODE_IRQ | IRQ_DISABLE | FIQ_DISABLE)
    ldr     sp, =__irq_stack_top__
    push	{r0}

    @ ABT (Abort) mode
    msr     CPSR_c, #(MODE_ABT | IRQ_DISABLE | FIQ_DISABLE)
    ldr     sp, =__abt_stack_top__
    push	{r0}

    @ UND (Undefined) mode
    msr     CPSR_c, #(MODE_UND | IRQ_DISABLE | FIQ_DISABLE)
    ldr     sp, =__und_stack_top__
    push	{r0}

    @ SVC mode
    msr     CPSR_c, #(MODE_SVC | IRQ_DISABLE | FIQ_DISABLE)
    ldr     sp, =__svc_stack_top__
    push	{r0}

    @ SYS mode
    msr     CPSR_c, #(MODE_SYS)
    ldr     sp, =__sys_stack_top__
    push	{r0}


init_memory_sys:
	/********************************************************************
	 * 3-Initialize the memory system, including the MMU.
	 */
	ldr		r12, =init_memory_system
	blx		r12

	@ Enable FPU
	ldr		r12, =enable_neon_fpu
	blx		r12





primary_cpu_init:
@init_exception_vectors:
	/********************************************************************
	 * 2-Initialize exception vectors.
	 * If the system is being initialized from the SD card (for example)
	 * copy the vector table to OCRAM and redifine the Vector Base
	 * Address Register.
	 */
	@ copy the vector table into the RAM vectors
   	ldr		r1, =__vectors_start
    mov		r11, r1						@ Save copy
    ldr		r2, =__vectors_end
	ldr		r3, =vectors

copy_vector_table:
    cmp     r1,r2
    ldmlt   r3!,{r4,r5,r6}
    stmlt   r1!,{r4,r5,r6}
    blt     copy_vector_table

	@ Set Vector Base Address Register (VBAR) to point to this application's vector table
	MCR     p15, 0, r11, c12, c0, 0    @ Write Secure or Non-secure Vector Base Address
	MRC     p15, 0, r1, c1, c0, 0      @ Read SCTLR
	BIC     r1, #(1<<13)               @ Clear V-bit
	MCR     p15, 0, r1, c1, c0, 0      @ Write SCTLR


zero_bss:
	/********************************************************************
	 * Clear .bss section.
	 */
    ldr		r12, =clear_bss
    blx		r12


init_static_constructors:
	/********************************************************************
	 * Initialize static constructors.
	 */
	ldr		r12, =constructors_init
	blx		r12


call_main:
	/********************************************************************
	 * Main function. -> RODOS
	 */
	ldr     r12, =main			@ save this in register for possible long jump
    blx     r12					@ branch to main()

    @ If ever return
    b		.

    .endfunc    				@ _start()



/*************************************************
  void clear_bss()
*************************************************/
    .global clear_bss
    .func clear_bss
clear_bss:
	@ clear the .bss section (zero init)
    ldr     r1,=__bss_start
    ldr     r2,=__bss_end
    mov     r3,#0
1:
    cmp     r1,r2
    stmltia r1!,{r3}
    blt     1b
    bx		lr
	.endfunc

.end
