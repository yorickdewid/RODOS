/****************************************************************************
 **
 ** Copyright (c) 2012 David Welch dwelch@dwelch.com
 ** with portions Copyright (c) 2013 University Wuerzburg
 ** and German Aerospace Center (DLR)
 **
 ** Permission is hereby granted, free of charge, to any person obtaining a
 ** copy of this software and associated documentation files (the "Software"),
 ** to deal in the Software without restriction, including without limitation
 ** the rights to use, copy, modify, merge, publish, distribute, sublicense,
 ** and/or sell copies of the Software, and to permit persons to whom the
 ** Software is furnished to do so, subject to the following conditions:
 **
 ** The above copyright notice and this permission notice shall be included
 ** in all copies or substantial portions of the Software.
 **
 ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 ** OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 ** DEALINGS IN THE SOFTWARE.
 **
 ****************************************************************************/

/**
 * @file vector.S
 * @date 2013/06/01
 * @author Johannes Freitag, David Welch
 *
 * Copyright 2013 DLR and University Wuerzburg
 */


.section "vectors"
.globl _start
_start:
    ldr pc,reset_handler
    ldr pc,undefined_handler
    ldr pc,swi_handler
    ldr pc,prefetch_handler
    ldr pc,data_handler
    ldr pc,unused_handler
    ldr pc,irq_handler
    ldr pc,fiq_handler
reset_handler:      .word reset
undefined_handler:  .word hang
swi_handler:        .word swint
prefetch_handler:   .word hang
data_handler:       .word hang
unused_handler:     .word hang
irq_handler:        .word irq
fiq_handler:        .word hang

reset:
    ;@ enable fpu
    mrc p15, 0, r0, c1, c0, 2  ;@ read Coprocessor Access Control Register
    orr r0,r0,#0x300000        ;@ single precision
    orr r0,r0,#0xC00000        ;@ double precision
    mcr p15, 0, r0, c1, c0, 2
    mov r0,#0x40000000         ;@ VFP enable bit
    fmxr fpexc,r0              ;@ set VFP enable bit in VFP11 Exception Register

    ;@copy 0x40 bytes from 0x8000 to 0x0000
    mov r0,#0x8000
    mov r1,#0x0000
    ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
    stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}
    ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
    stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}

    ;@ define stacks for processor modes

    ;@ (PSR_IRQ_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD2
    msr cpsr_c,r0
    mov sp,#0x4000

    ;@ (PSR_SVC_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD3
    msr cpsr_c,r0
    mov sp,#0x5000

    ;@ (PSR_SYS_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xDF
    msr cpsr_c,r0
    mov sp,#0x8000

    ;@ initialisations
    bl kernel_init

    ;@ call main
    bl main

hang: b hang

;@ write to memory
.globl PUT32
PUT32:
    str r1,[r0]
    bx lr

;@ read from memory
.globl GET32
GET32:
    ldr r0,[r0]
    bx lr

;@ read cpsr
.globl GETcpsr
GETcpsr:
    mrs r0,cpsr
    bx lr

;@ read lr
.globl GETlr
GETlr:
    mov r1,lr
    pop {lr}
    push {lr}
    mov r0,lr
    bx r1

.globl dummy
dummy:
    bx lr

;@ activates interrupts global
.globl enable_irq
enable_irq:
    mrs r0,cpsr
    bic r0,r0,#0x80
    msr cpsr_c,r0
    bx lr

;@ deactivates interrupts global
.globl disable_irq
disable_irq:
    mrs r0,cpsr
    orr r0,r0,#0x80
    msr cpsr_c,r0
    bx lr

;@ activates level 1 cache
.globl start_l1cache
start_l1cache:
    mov r0, #0
    mcr p15, 0, r0, c7, c7, 0 ;@ invalidate caches
    mcr p15, 0, r0, c8, c7, 0 ;@ invalidate tlb
    mrc p15, 0, r0, c1, c0, 0 ;@ Read Control Register configuration data     ARM1176JZF-S r0p7 3-44
    orr r0,r0,#0x1000 ;@ instruction
    orr r0,r0,#0x0004 ;@ data
    orr r0,r0,#0x0800 ;@ branch prediction
    mcr p15, 0, r0, c1, c0, 0 ;@ Write Control Register configuration data
    bx lr

;@ activates mmu
.globl start_mmu
start_mmu:
    mov r2,#0
    mcr p15,0,r2,c7,c7,0 ;@ invalidate caches
    mcr p15,0,r2,c8,c7,0 ;@ invalidate tlb

    mvn r2,#0
    mcr p15,0,r2,c3,c0,0 ;@ domain

    mcr p15,0,r0,c2,c0,0 ;@ tlb base
    mcr p15,0,r0,c2,c0,1 ;@ tlb base

    mrc p15,0,r2,c1,c0,0
    orr r2,r2,r1
    mcr p15,0,r2,c1,c0,0

    bx lr

;@ on interrupt
irq:
    b c_irq_handler

;@ on software interrupt
swint:
    b contextSwitch

.globl bss_start
bss_start: .word __bss_start__
.globl bss_end
bss_end: .word __bss_end__
