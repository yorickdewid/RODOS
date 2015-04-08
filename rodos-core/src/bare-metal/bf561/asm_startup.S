#include <sys/platform.h>
#include <cplb.h>
#include <sys/anomaly_macros_rtl.h>

//.text
.align 2;
.global __start;
.type __start, STT_FUNC;
.section .init;
__start:
	// Zap loop counters to zero, to make sure that
	// hw loops are disabled - it could be really baffling
	// if the counters and bottom regs are set, and we happen
	// to run into them.
	R7 = 0;
	LC0 = R7;
	LC1 = R7;

	// Clear the DAG Length regs too, so that it's safe to
	// use I-regs without them wrapping around.
	L0 = R7;
	L1 = R7;
	L2 = R7;
	L3 = R7;

	// Zero ITEST_COMMAND and DTEST_COMMAND
	// (in case they have crud in them and
	// does a write somewhere when we enable cache)
	I0.L = (ITEST_COMMAND & 0xFFFF);
	I0.H = (ITEST_COMMAND >> 16);
	I1.L = (DTEST_COMMAND & 0xFFFF);
	I1.H = (DTEST_COMMAND >> 16);
	R7 = 0;
	[I0] = R7;
	[I1] = R7;
	// It seems writing ITEST_COMMAND from SDRAM with icache enabled
	// needs SSYNC.
	ssync;

	//set stack pointer
	SP.L = __stack_end -12;
	SP.H = __stack_end -12;

	usp = sp;
	fp = sp;
	sp += -12;

	R7.L = 0xffff
	P0.L = (ILAT & 0xffff );
	P0.H = (ILAT >> 16);
	[P0] = R7;

	P0.H = _init_pll;
	P0.L = _init_pll;
	call (P0);
	P0.H = _chip_init;
	P0.L = _chip_init;
	call (P0);
	P0.H = _bss_init;
	P0.L = _bss_init;
	call (P0);
	P0.H = _cpp_constructors;
	P0.L = _cpp_constructors;
	call (P0);

	//The code has to be initialised in supervicor mode
	P0.L = (EVT15 & 0xffff);
	P0.H = (EVT15 >> 16);
	P1.L = start;
	P1.H = start;
	[P0] = P1; //set interrupt vector 15 to start

	//enable interrupt 15 all other interrupts deactivated, will be activated in init process
	P0.L = (IMASK & 0xffff);
	P0.H = (IMASK >> 16);
	R0.L = EVT_IVG15 ;
	[P0] = R0;
	ssync;
	RAISE 15; //software interrupt will be raised

	P0.L = wait_interrput ;
	P0.H = wait_interrput ;
	RETI = P0;

//all interrupts are serviced until interrupt 15 is raised
wait_interrput:
	RTI;

start:
	[--SP] = RETI; //find out why to save this on the stack
	P0.L = _main;
	P0.H = _main;
	call (P0);
