/*
 * @file	hw_abort.cpp
 * 
 * @author	Fabian Greif
 * @date	June 2011
 */

#include <stdio.h>

#include "yprintf.h"
#include "lpc24xx_registers.h"

#include "debug.h"
#include "hw_abort.h"
#include "constants.h"
extern uint32_t __stack_start;		// set by the linkerscript
extern uint32_t __stack_end;		// set by the linkerscript

namespace RODOS {
extern "C"
{
	void _abort_handler(const AbortContext* ctx) __attribute__ ((noreturn));
	void _abort_handler(const AbortContext* ctx)
	{
		switch (ctx->abort_id)
		{
		case 0:
			xprintf("\nUndefined Instruction:\n");
			break;
		case 1:
			xprintf("\nPrefetch Abort:\n");
			break;
		case 2:
			xprintf("\nData Abort:\n");
			break;
		default:
			xprintf("\nUnknown Exception:\n");
			break;
		}
		
		// Step 1. Print a Register dump
		for (int i = 0; i < 8; i++)
		{
			xprintf("r%d : 0x%08lx   ", i, ctx->reg[i]);
			xprintf("r%d%s: 0x%08lx\n", i + 8, i + 8 < 10 ? " " : "", ctx->reg[i + 8]);
		}
		
		// Step 2. Print status flags
		xprintf("PSR: 0x%08lx  ", ctx->cpsr);
		
		// now decode the flags
		uint32_t cpsr = ctx->cpsr;
		putchar((cpsr & CPSR_N_BIT) ? 'N' : 'n');
		putchar((cpsr & CPSR_Z_BIT) ? 'Z' : 'z');
		putchar((cpsr & CPSR_C_BIT) ? 'C' : 'c');
		putchar((cpsr & CPSR_V_BIT) ? 'V' : 'v');
		xprintf("...");
		putchar((cpsr & CPSR_F_BIT) ? 'F' : 'f');
		putchar((cpsr & CPSR_I_BIT) ? 'I' : 'i');
		putchar((cpsr & CPSR_T_BIT) ? 'T' : 't');
		
		const char *mode;
		switch (MODE_MASK & cpsr)
		{
		case MODE_USR:
			mode = "user";
			break;
		case MODE_FIQ:
			mode = "fiq";
			break;
		case MODE_IRQ:
			mode = "irq";
			break;
		case MODE_SVC:
			mode = "supervisor";
			break;
		case MODE_SYS:
			mode = "system";
			break;
		default:
			mode = 0;
			break;
		}
		if (mode) {
			xprintf(", %s-mode\n", mode);
		}
		// Step 3. Try to generate a Backtrace
		xprintf("\nBacktrace:\n");
		xprintf(" 0) 0x%08lx\n", ctx->pc);
		// TODO
		
		// Step 4. Print a Stack dump
		xprintf("\nStackpointer:\n");
		xprintf("usr: 0x%08lx\n", ctx->sp_usr);
		xprintf("fiq: 0x%08lx\n", ctx->sp_fiq);
		xprintf("irq: 0x%08lx\n", ctx->sp_irq);
		xprintf("svc: 0x%08lx\n", ctx->sp_svc);
		xprintf("abt: 0x%08lx\n", (uint32_t) ctx);
		
		xprintf("\nStack dump:");
		uint32_t counter = -1;
		const uint32_t *ptr = &__stack_start;
//		const uint32_t *ptr = (const uint32_t *) 0x40000000;
		while (ptr != &__stack_end)
		{
			counter++;
			if ((counter % 16) == 0) {
				xprintf("\n%8p : ", (void *) ptr);
			}
			xprintf("%08lx ", *ptr++);
		}
		
		// Step 5. Infinite loop
		while (1)
		{
			led_red_on();
			led_green_off();
			for (volatile int j = 0; j < 200000; ++j) {
			}
			led_red_off();
			led_green_on();
			for (volatile int j = 0; j < 200000; ++j) {
			}
		}
	}
}
}
