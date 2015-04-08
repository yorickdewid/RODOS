/**
* @file hw_specific.cc
* @date 2008/04/23 7:33
* @port 2013/03/12 17:50
* @author Tiago Duarte
*
* Copyright 2010 DLR
*
*/

/************************************/
#include <sys/stat.h>
#include <errno.h>

// RODOS includes
#include "params.h"
#include "hw_specific.h"  //bare-metal-generic
#include "context.h"
#include "rodos.h"

// Work space includes
#include "workspace.h"


extern "C"{
	void asmEnabelIRQ();
	void asmDisabelIRQ();
}

void initDebugOutput();

TContext context_to_be_saved;
unsigned int context_pointer= 0xDEADBEEF;

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/* Initially we have no threads */
long volatile taskRunning;


/*-----------------------------------------------------------*/
/* It is only used on processors that support special
 * exceptions for context switching.
 */
void startIdleThread(){

}

/*-----------------------------------------------------------*/
/* This is the 1st function to be called after main().
 * This should initialize the uart, timer, etc.
 */
void hwInit(){
	disable_strict_align_check();

	// At this time we don't have threads running!
	taskRunning = 0;

	if(cpu_get_current()==0){
		ccm_init();
	}
	initDebugOutput();


	// Enable interrupts
	gic_enable(true);					// Enable both secure and non-secure.
	gic_cpu_enable(true);
	gic_set_cpu_priority_mask(255);

	// Print information about the Architecture and CPU
	xprintf("\n\n");
	xprintf("*********************************************************\n");
	xprintf("****************** RODOS in Nitrogen6X ******************\n");
	xprintf("*********************************************************\n");
//	xprintf("CPU clock      : %d MHz\n", (int)get_main_clock(CPU_CLK)/1000000);
//	xprintf("AXI clock      : %d MHz\n", (int)get_main_clock(AXI_CLK)/1000000);
//	xprintf("AHB clock      : %d MHz\n", (int)get_main_clock(AHB_CLK)/1000000);
//	xprintf("IPG clock      : %d MHz\n", (int)get_main_clock(IPG_CLK)/1000000);
//	xprintf("IPG_PER clock  : %d MHz\n", (int)get_main_clock(IPG_PER_CLK)/1000000);
	xprintf("Current CPU: %d\n", cpu_get_current());

//	xprintf("Main ID information:\n");
//	unsigned long mainID;
//	_ARM_MRC(15, 0, mainID, 0, 0, 0);
//	xprintf("  Minor revision: %d;", (int)mainID & 0xf);
//	xprintf(" Part number: %x;", (unsigned int)(mainID & 0xfff0)>>4);
//	xprintf(" Architecture: %x\n", (unsigned int)(mainID & 0xf0000)>>16);
//	xprintf("  Major revision: %d;", (int)(mainID & 0xf00000)>>20);
//	xprintf(" Implementer: %s\n", ((mainID & 0xff000000)>>24)==0x41 ? "ARM Limited" : "Unknown");
//
//	xprintf("SCU (Snoop Control Unit) configurations:\n");
//	unsigned long SCU;
//	asm volatile (
//			"mrc	p15, 4, %0, c15, c0, 0     \n\t"
//			"ldr	%0, [%0, #0x004] \n\t"
//			: "=r"(SCU)       // Output registers
//	);
//	xprintf("  TAG RAM size for:\n");
//	xprintf("    CPU 0: %d KB\n", ((SCU & 0x300)>>8) == 0 ? 16 :
//												 ((SCU & 0x300)>>8) == 1 ? 32 : 64);
//	xprintf("    CPU 1: %d KB\n", ((SCU & 0xc00)>>10) == 0 ? 16 :
//												 ((SCU & 0xc00)>>10) == 1 ? 32 : 64);
//	xprintf("    CPU 2: %d KB\n", ((SCU & 0x3000)>>12) == 0 ? 16 :
//												 ((SCU & 0x3000)>>12) == 1 ? 32 : 64);
//	xprintf("    CPU 3: %d KB\n", ((SCU & 0xc000)>>14) == 0 ? 16 :
//												 ((SCU & 0xc000)>>14) == 1 ? 32 : 64);
//
//	xprintf("GIC (Generic Interrupt Controller) information:\n");
//	xprintf(" CPU interface:\n");
//	gicc_t * gicc = (gicc_t *)(get_arm_private_peripheral_base() + kGICCBaseOffset);
//	unsigned int reg = gicc->IIDR;
//	xprintf("  Product ID: %d;", (reg & 0xfff00000)>>20);
//	xprintf(" Arch. version: %d;", (reg & 0x000f0000)>>16);
//	xprintf(" Revision: %d\n", (reg & 0x0000f000)>>12);
//	xprintf(" Distributor:\n");
//	gicd_t * gicd = (gicd_t *)(get_arm_private_peripheral_base() + kGICDBaseOffset);
//	reg = gicd->IIDR;
//	xprintf("  Product ID: %d;", (reg & 0xff000000)>>24);
//	xprintf(" Variant: %d;", (reg & 0x000f0000)>>16);
//	xprintf(" Revision: %d;", (reg & 0x0000f000)>>12);
//	reg = gicd->TYPER;
//	xprintf(" SecurityExtn: %d\n", (reg & (1<<10))>>10);
//	xprintf("  Number of CPU's: %d;", ((reg & (7<<5))>>5) + 1);
//	xprintf(" Number of IT's: %d\n", 32*((reg & 0x0000001f) + 1));
//
//	xprintf("DEBUG information (Core%d):\n", cpu_get_current());
//	_ARM_MRC(14, 0, reg, 0, 0, 0);
//	xprintf("  Watchpoints: %d;", ((reg & 0xf0000000)>>28) + 1);
//	xprintf(" Breakpoints: %d;", ((reg & 0x0f000000)>>24) + 1);
//	int aux = (reg & 0x000f0000)>>16;
//	xprintf(" Version: ARMv%s\n", aux == 1 ? "6"   :
//										  aux == 2 ? "6.1" :
//										  aux == 3 ? "7 with CP14 regs." :
//										  aux == 4 ? "7 with only the baseline CP14 regs." :
//										  aux == 5 ? "7.1" : "--");
//	xprintf("  Variant: %d;", (reg & 0x000000f0)>>4);
//	xprintf(" Revision: %d\n", reg & 0x0000000f);
//	_ARM_MRC(14, 0, reg, 1, 0, 0);
//	xprintf("  Debug ROM addr: %x - %s\n", reg & 0xfffff000, (reg & 0x3) == 3 ? "valid" : "not valid");
//	_ARM_MRC(14, 0, reg, 2, 0, 0);
//	xprintf("  Debug self addr offset: %x - %s\n", reg & 0xfffff000, (reg & 0x3) == 3 ? "valid" : "not valid");
//
////	xprintf("Floating Point Unit:\n");
////	asm volatile("vmrs	%0, fpsid \t\n" : "=r"(reg) );
////	xprintf(" %x\n", reg);
//
//	_ARM_MRC(15, 0, reg, 12, 0, 0);
//	xprintf("VBA: %x\n", reg);
	extern long myNodeNr;
	myNodeNr = 0x1234 + cpu_get_current();

}

extern "C" void secondary_hwInit(){
	hwInit();
}


/*-----------------------------------------------------------*/
/* This function is called by initSystem(), in main(),
 * and it should initialize the process for time in RODOS.
 * initSystem is the 2nd function to be called in main().
 *
 */
void hwInitTime(){
	// Enable the timer for RODOS time, but only the CPU0 was this authority
	if(cpu_get_current() == 0)
		global_timer_start();
}


/*-----------------------------------------------------------*/
/* Returns the number of passed nanoseconds since the
 * system was booted up.
 */
unsigned long long hwGetNanoseconds(){
	return global_timer_get_value();
}


/*-----------------------------------------------------------*/
/* While time events are mostly based on the value returned
 * by this function, a pseudo random number generator greatly
 * benefits from increased entropy and relies on
 * hwGetAbsoluteNanoseconds() which adds the value of an
 * internal real-time clock if present. Otherwise, it just
 * returns the same value as hwGetNanoseconds().
 */
unsigned long long hwGetAbsoluteNanoseconds(){
	return global_timer_get_value();
}



/*-----------------------------------------------------------*/
/* Abort execution. Call the _start function - the entry point
 * for system (iMX6) initialization. This causes a reboot.
 */
void hwResetAndReboot() {
}

/**
  *  global interrupt disable
  */
void hwDisableInterrupts(){
   asmDisabelIRQ();
}

/**
  *  global interrupt enable
  */
void hwEnableInterrupts(){
    asmEnabelIRQ();
}


/*-----------------------------------------------------------*/
/* It is only used on top of other operating systems such as
 * Linux to allow additional space/time partitions in terms
 * of processes.
 */
void sp_partition_yield(){
	return;
}


/*-----------------------------------------------------------*/
/*
 * Create context on stack and return a pointer to it
 * For every thread this function is called.
 *
 * @ params:
 * -> *stack is the pointer to the thread's stack and provides the basic to
 * compute an appropriate stack pointer.
 * -> *object is the thread's object pointer that is passed to the callback function.
 * It's stored in the register that makes up the first argument on a function call.
 */
long* hwInitContext(long* stack, void* object) {

	long* tos = stack;		/* Top Of Stack */
	*tos = 0xdeaddead;		/* debug */
	TContext *ctx = (TContext*) (tos - sizeof(TContext)/sizeof(long));

	ctx->r0 = (long) object;
	ctx->r1 = 0;
	ctx->r2 = 0;
	ctx->r3 = 0;
	ctx->r4 = 0;
	ctx->r5 = 0;
	ctx->r6 = 0;
	ctx->r7 = 0;
	ctx->r8 = 0;
	ctx->r9 = 0;
	ctx->r10 = 0;
	ctx->r11 = 0;
	ctx->r12 = 0;
	ctx->SP = ((long) ctx)-8;
	ctx->LR = 0;
	ctx->PC = (long) threadStartupWrapper;	/* this method is defined in /bare-meta-generic/thread_on_hw.cpp */

	/* ctx->CPSR = CPSR */
	asm volatile (
		"mrs  %0, CPSR     \n\t"
		: "=r"(ctx->CPSR)       // Output registers
	);


	return (long*) ctx;
}


/*-----------------------------------------------------------*/
/*
 */

#ifndef NO_RODOS_NAMESPACE
}
#endif

