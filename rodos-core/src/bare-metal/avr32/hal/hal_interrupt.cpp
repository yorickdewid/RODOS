/*
 * Copyright (c) 2012 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Michael Ruffer
 */
//#include "rodos.h"

extern "C" {
	#include "intc.h"
}

#include "hal_interrupt.h"


void HW_setInterruptHandler(IRQ_IDX index, IRQ_PRIO prio, void (*interruptHandler)()){
	INTC_register_interrupt(interruptHandler, index, prio);
}


void HW_enableInterrupts(){
	Enable_global_interrupt();
}


void HW_disableInterrupts(){
	Disable_global_interrupt();
}


void HW_maskInterrupts(IRQ_PRIO prio){
	switch (prio){
		case 0:
			__asm__ __volatile__ ("ssrf %0" :  : "i" (AVR32_SR_I0M_OFFSET)); break;
		case 1:
			__asm__ __volatile__ ("ssrf %0" :  : "i" (AVR32_SR_I1M_OFFSET)); break;
		case 2:
			__asm__ __volatile__ ("ssrf %0" :  : "i" (AVR32_SR_I2M_OFFSET)); break;
		case 3:
			__asm__ __volatile__ ("ssrf %0" :  : "i" (AVR32_SR_I3M_OFFSET)); break;
	}
}


void HW_unmaskInterrupts(IRQ_PRIO prio){
	switch (prio){
		case 0:
			__asm__ __volatile__ ("csrf %0" :  : "i" (AVR32_SR_I0M_OFFSET)); break;
		case 1:
			__asm__ __volatile__ ("csrf %0" :  : "i" (AVR32_SR_I1M_OFFSET)); break;
		case 2:
			__asm__ __volatile__ ("csrf %0" :  : "i" (AVR32_SR_I2M_OFFSET)); break;
		case 3:
			__asm__ __volatile__ ("csrf %0" :  : "i" (AVR32_SR_I3M_OFFSET)); break;
	}
}

// will be increased in  _unhandled_interrupt() defined in avr32_framework/intc.c
extern unsigned long unhandledInterruptCounter;

unsigned long HW_getUnhandledInterrupCounter(){
	return unhandledInterruptCounter;
}
