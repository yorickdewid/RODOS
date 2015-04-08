/* ------------------------------------------------------------------------- */
/**
 * @brief	Startup code for ARMv4
 * 
 * As this code uses the VIC for NXP it will only work for LPC23xx and LPC24xx
 * devices!
 */
/* ------------------------------------------------------------------------- */

	#include "constants.h"
	
	.section .startup
	.arm
	.align 4
	
	.global	_startup
	.func	_startup
_startup:

/* ------------------------------------------------------------------------- */
/* Exception Vectors */
_vectors:
	ldr		pc, reset_addr
	ldr		pc, undef_addr
	ldr		pc, swi_addr
	ldr		pc, pabt_addr			/* Prefetch Abort */
	ldr		pc, dabt_addr			/* Data Abort => during data access */
	.word	0xb8a06f60				/* Reserved vector (valid checksum to make openocd's verify happy) */
	ldr		pc, irq_addr
	ldr		pc, fiq_addr

reset_addr:	.word reset_handler
undef_addr:	.word undef_handler
swi_addr:	.word swi_handler
pabt_addr:	.word pabt_handler
dabt_addr:	.word dabt_handler
irq_addr:	.word irq_handler
fiq_addr:	.word fiq_handler

/* ------------------------------------------------------------------------- */
	. = 0x40
reset_handler:
	/* setup stack for each mode */
	/* for simplicity all use same stack, because there is no return !!! */

	/* Fill the .stack section with a known pattern */
	ldr		r0, =__stack_start
	ldr		r1, =__stack_end
	ldr		r2, =STACK_FILL
1:
	cmp		r0, r1
	strlo	r2, [r0], #4
	blo		1b

	/* IRQ and FIQ interrupts disabled by default for
	 * all modes except user mode */
	/* The __xxx_stack_top entries are set by the linker (see linkerscript.ld) */
	ldr r0, =END_OF_STACK_MARKER
	
	/* Undefined Instruction Mode and Abort Mode use the same stack */
	msr		CPSR_c, #MODE_UND | IRQ_DISABLE | FIQ_DISABLE
	ldr		sp, =__abt_stack_top
	msr		CPSR_c, #MODE_ABT | IRQ_DISABLE | FIQ_DISABLE
	ldr		sp, =__abt_stack_top
	stmfd	sp!, { r0 }
	
	/* FIQ Mode (used for context switching by RODOS) */
	msr		CPSR_c, #MODE_FIQ | IRQ_DISABLE | FIQ_DISABLE
	ldr		sp, =__fiq_stack_top
	stmfd	sp!, { r0 }
	
	/* IRQ Mode (used for all the other peripheral interrupts) */
	msr		CPSR_c, #MODE_IRQ | IRQ_DISABLE | FIQ_DISABLE
	ldr		sp, =__irq_stack_top
	stmfd	sp!, { r0 }
	
	/* Supervisor Mode */
	msr		CPSR_c, #MODE_SVC | IRQ_DISABLE | FIQ_DISABLE
	ldr		sp, =__svc_stack_top
	stmfd	sp!, { r0 }
	
	/* User Mode => Interrupts enabled */
	msr		CPSR_c, #MODE_SYS
	ldr		sp, =__usr_stack_top
	stmfd	sp!, { r0 }
	
	/* copy data from ROM to RAM */
	ldr		r1, =_etext
	ldr		r2, =_data
	ldr		r3, =_edata
1:
	cmp		r2, r3
	ldrlo	r4, [r1], #4
	strlo	r4, [r2], #4
	blo		1b

	/* clear .bss section */
	mov		r0, #0
	ldr		r2, =_bss_start
	ldr		r3, =_bss_end
2:
	cmp		r2, r3
	strlo	r0, [r2], #4
	blo		2b
	
	/* call main function */
	bl		main
3:
	b		3b
.endfunc

/* ------------------------------------------------------------------------- */
/* Used by the RODOS context switching to change into a priviledged mode => Supervisor mode */
swi_handler:
	bx		lr

/* ------------------------------------------------------------------------- */
/**
 * @brief	Assembler interrupt handler wrapper
 * 
 * Allows that interrupts are written as normal C functions for the
 * trade-off of some additional CPU cycles. Saves and restores used
 * registers. 
 * 
 * A subroutine must preserve the content of the registers r4-r11 and
 * sp (r13), therefore the C interrupt handler functions is responsible
 * for restoring them.
 * 
 * r0-r3 are used for arguments and must be saved by this handler,
 * r12-r15 have special roles:
 * 
 * r12 - ip (Intra-Procedure-call scratch register, must be saved)
 * r13 - sp (Stack pointer, banked)
 * r14 - lr (Link register, banked. Used as temporary register after saving it)
 * r15 - pc (program counter)
 * 
 *      |                   | <--- irq stack
 *      +-------------------+ 
 *      |     lr (r14)      |
 *      +-------------------+ 
 *      |       SPSR        |
 *      +-------------------+ 
 *      |        r0         | <--- branch to svc_mode
 *      +-------------------+ 
 *      |                   | 
 *
 *
 *      |                   | <--- svc_stack
 *      +-------------------+ 
 *      |        r14        | 
 *      +-------------------+ 
 *      |        r12        |
 *      +-------------------+ 
 *      |        r3         |
 *      +-------------------+ 
 *      |        r2         |
 *      +-------------------+ 
 *      |        r1         | <--- branch to interrupt handler routine from VIC
 *      +-------------------+ 
 *      |                   |
 *
 * @author	Fabian Greif
 * @date	June 2011
 */
irq_handler:
	/* disable FIQ interrupt */
	msr		CPSR_c, #MODE_IRQ | IRQ_DISABLE | FIQ_DISABLE
	
	/* Adjust return address and push it on the stack */
	sub		lr, lr, #4
	stmfd	sp!, {lr}
	
	/* Push used registers (r0) and SPSR (in r14 = lr) */
	mrs		r14, SPSR
	stmfd	sp!, {r0, r14}
	
	/* Get the ISR address pointed by VIC_VectAddr */
	ldr		r14, =VIC_BASE
	ldr		r0, [r14, #VIC_VectAddr]
	
	/* Switch to Supervisor Mode
	 * 
	 * A mode switch is needed if nested interrupts should be allowed. Otherwise
	 * the next interrupt would destroy the current link register (r14).
	 */
	msr		CPSR_c, #MODE_SVC | IRQ_DISABLE | FIQ_DISABLE
	
	/* Save scratch/used registers and LR in supervisor stack */
	stmfd	sp!, { r1-r3, r12, r14 }
	
	/* Branch to the routine pointed by the VIC_VectAddr */
	mov		lr, pc
	bx		r0
	
	/* Restore scratch/used registers and LR from supervisor stack */
	ldmfd	sp!, { r1-r3, r12, r14 }
	
	/* Disable Interrupt and switch back in IRQ mode */
	msr		CPSR_c, #MODE_IRQ | IRQ_DISABLE | FIQ_DISABLE
	
	/* Acknowledge interrupt => Issue EOI command to the VIC */
	ldr		r14, =VIC_BASE
	str		r14, [r14, #VIC_VectAddr]
	
	/* Restore SPSR_irq and r0 from IRQ stack */
	ldmfd	sp!, {r0, r14}
	msr		SPSR_cxsf, r14
	
	/* Restore adjusted  LR_irq from IRQ stack directly in the PC */
	ldmfd	sp!, {pc}^

/* ------------------------------------------------------------------------- */
/**
 * @brief	FIQ handler
 * 
 * Called by a timer interrupt. Switches the context to the next
 * thread ready to work.
 * 
 * r8-r14 are banked and don't need to be saved here.
 *
 * @author	David Waleczek
 * @author	Fabian Greif
 * @date	June 2011
 */
fiq_handler:
	/* The IRQ handler disables the FIQ at one of the first instructions.
	 * However there is small unavoidable gap in which the IRQ is executed
	 * with FIQ enabled.
	 * During this time the FIQ is not able to do a context switch as no
	 * thread context is active. So the only thing to do is to disable the FIQ
	 * for the IRQ mode and return into the IRQ handler. As the Timer Interrupt
	 * flag is not cleared the interrupt cause is still active.
	 * Therefore the FIQ is executed again as soon as the IRQ is finished.
	 */
	mrs		r8, SPSR				/* load status register of the interrupted mode */
	and		r9, r8, #0x1f
	cmp		r9, #MODE_IRQ			/* check if mode is IRQ */
	orreq	r8, r8, #FIQ_DISABLE	/* disable FIQ for the IRQ mode */
	msreq	SPSR_cxsf, r8
	subeqs	pc, lr, #4				/* return to the IRQ handler */
	
	/* clear interrupt => T0IR = ~0 */
	ldr		r8, =T0IR
	ldr		r9, =-1
	str		r9, [r8]

	/* get user mode stack pointer */
	sub		sp, sp, #4			/* allocate space */
	stmia	sp, {sp}^			/* save user mode stack pointer */
	ldmfd	sp!, {r12}			/* r12 = stack pointer of user mode */

	/* adjust and save return address on user stack */
	sub		lr, lr, #4
	stmfd	r12!, {lr}
	
	/* TODO: copy SPSR of the current mode to the CPSR (see 'movs', ARM ARM A4.1.35) */
	ldr		r12, =fiq_to_swi
	movs	pc, r12
fiq_to_swi:
	
	/* enter supervisor mode */
	swi		0
	
	/* get user mode stack pointer */
	sub		sp, sp, #4			/* allocate space */
	stmia	sp, {sp}^			/* save user mode stack pointer */
	ldmfd	sp!, {lr}			/* lr = stack pointer of user mode */
	
	sub		lr, lr, #4			/* pc was stored above */
	b		__asmSaveContext	/* branch to context saving code */
	
/* ------------------------------------------------------------------------- */
/**
 * @brief	Abort Handler
 * 
 * These handlers are called directly form the vector table and save the abort
 * cause and the corrected return address and branch to a common handler.
 * This common handler collectes the remaining context of the currently
 * executed mode.
 * 
 * To display a report (via UART) a C function called _abort_handler() is
 * called with a pointer to the abort context as first parameter.
 * 
 * By default an abort is expected to be an unrecoverable program fault,
 * therefore this function must not return. It it does the CPU operation is
 * stopped by disabling interrupts and executing an infinite loop.
 * 
 * In production code the default _abort_handler() may be replaced by a
 * function to restart the processor in the hope that the abort was caused
 * by a temporary fault. 
 * 
 * @author	Fabian Greif
 * @date	June 2011
 */
undef_handler:
	sub		sp, sp, #4 * 4
	stmia	sp!, {lr}
	sub		lr, lr, #4		/* ajust lr to point to the originating command */
	stmia	sp!, {lr}
	mov		lr, #0x0
	b		common_handler

pabt_handler:
	sub		sp, sp, #4 * 4
	stmia	sp!, {lr}
	sub		lr, lr, #4		/* ajust lr to point to the originating command */
	stmia	sp!, {lr}
	mov		lr, #0x1
	b		common_handler
	
dabt_handler:
	sub		sp, sp, #4 * 4
	stmia	sp!, {lr}
	/* ajust lr to point to the originating command.
	 * 
	 * The instruction that caused the abort is at lr_ABT – 8 because lr_ABT
	 * points two instructions beyond the instruction that caused the abort. */
	sub		lr, lr, #8
	stmia	sp!, {lr}
	mov		lr, #0x2
	b		common_handler
	
common_handler:
	stmia	sp!, {lr}
	mrs		lr, SPSR
	stmia	sp!, {lr}
	
	/* allocate space for current context and save the lower registers
	 * => lower registers are the same for all modes (not banked) */
	sub		sp, sp, #(19 * 4)
	stmia	sp, {r0-r7}
	
	/* save current mode in r0 and disable interrupts */
	mrs		r0, CPSR
	orr		r0, r0, #IRQ_DISABLE | FIQ_DISABLE
	msr		CPSR_c, r0
	
	/* save and ajust stackpointer before changing mode */
	add		r2, sp, #(8 * 4)
	mov		r3, sp
	
	/* Save r8-r14 from within originating mode (with interrupts disabled)
	 * to get the right value for banked registers */
	mrs		r1, SPSR
	tst		r1, #0xf
	orreq	r1, r1, #0xf			/* if mode is 'user' change to 'system' to stay in a privileged mode */
	orr		r1, r1, #IRQ_DISABLE | FIQ_DISABLE	/* disable IRQ and FIQ interrupt */
	bic		r1, r1, #THUMB_ENABLE	/* Thumb bit might be set, clear it => force ARM mode */
	msr		CPSR_c, r1
	stmia	r2, {r8-r14}
	
	/* Save current stack pointers form the different modes */
	msr		CPSR_c, #MODE_SVC | IRQ_DISABLE | FIQ_DISABLE
	stmfd	r3!, {sp}
	msr		CPSR_c, #MODE_IRQ | IRQ_DISABLE | FIQ_DISABLE
	stmfd	r3!, {sp}
	msr		CPSR_c, #MODE_FIQ | IRQ_DISABLE | FIQ_DISABLE
	stmfd	r3!, {sp}
	msr		CPSR_c, #MODE_SYS | IRQ_DISABLE | FIQ_DISABLE
	stmfd	r3!, {sp}
	
	msr		CPSR_c, r0				/* switch back */
	mov		sp, r3
	
	/* Call the exception handler to display the result.
	 * The handler should not return, go to an infinite loop if it does. */
	mov		r0, sp
	ldr		r1, =_abort_handler
	mov		lr, pc
	bx		r1
	b		.
	
// ----------------------------------------------------------------------------
/**
 * @brief	Save interrupt state and disable interrupts
 * 
 * C equivalent:
 * \code
 * extern "C" uint32_t _disable_interrupt(void);
 * \endcode
 * 
 * @author	Fabian Greif
 * @date	June 2011
 */
	.global	_disable_interrupt
	.func	_disable_interrupt
	.type	_disable_interrupt, %function
_disable_interrupt:
	mrs		r0,	CPSR
	orr		r1, r0, #IRQ_DISABLE | FIQ_DISABLE
	msr		CPSR_cxsf, r1
	bx		lr
	.endfunc

// ----------------------------------------------------------------------------	
/**
 * @brief	Restore interrupt state
 * 
 * C equivalent:
 * \code
 * extern "C" void _restore_interrupt(uint32_t cpsr);
 * \endcode
 * 
 * @author	Fabian Greif
 * @date	June 2011
 */
	.global	_restore_interrupt
	.func	_restore_interrupt
	.type	_restore_interrupt, %function
_restore_interrupt:
	msr		CPSR_cxsf, r0
	bx		lr
	.endfunc
	
	.end
