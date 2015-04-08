
#include <stdio.h>
#include <stdint.h>

#include "yprintf.h"
#include "lpc24xx_registers.h"
#include "lpc2468_pll.h"

#include "hw_debug_uart.h"
#include "debug.h"

#include "hw_specific.h"
extern unsigned long __start_ctors;
extern unsigned long __end_ctors;

namespace RODOS {
static void
default_interrupt_handler()
{
	xprintf("\nDefault Interrupt\n");
	
	while (1)
	{
		led_red_on();
		led_green_off();
		for (int j = 0; j < 100000; ++j)
		{
			asm volatile("nop");
		}
		led_red_off();
		led_green_on();
		for (int j = 0; j < 100000; ++j)
		{
			asm volatile("nop");
		}
	}
}

void
hwInit()
{
	MEMMAP = 0x01; // prevent remapping of interrupt vectors
	
	// Enable MAM => flash read modus: 128 bits per flash cycle
	MAMCR = 0;
	MAMTIM = 4;
	MAMCR = 2;
	
	initPLL();
	
	/* interrupt controller management */
	VICIntEnClear = ~0; 	// disable all interrupt sources
	VICSoftIntClear = ~0;	// clear pending software interrupts
	VICIntSelect = 0; 		// select IRQ for all interrupt sources
	
	// set a default handler for every interrupt
	uint32_t *vic_addr = (uint32_t *) &VICVectAddr0;
	for (unsigned int i = 0; i < 32; ++i) {
		*vic_addr++ = (uint32_t) default_interrupt_handler;
	}
	
	DebugUart::initialize();
	
	xprintf("calling constructors\n");
	// call all constructors
	unsigned long* call;
	for (call = &__start_ctors; call < &__end_ctors; call++) {
		((void (*)(void))*call)();
	}
	xprintf("calling constructors done\n");
	
	hwInitTime();
	
	Timer::init();
	
	FIO4DIR  =   3 << 16; /* set as ouput */
	FIO4MASK = ~(3 << 16);  /* enable pins */
	FIO4SET  =   3 << 16; /* clear pins */
}
}
