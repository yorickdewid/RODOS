/* -*- Mode: C++ -*- */

/**
* @file
* @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2008-2010 DLR
*
* @brief Implementation of everything that hw_specific.h expects.
*
* This also implements stuff in private extern declarations like
* taskRunning, which is found in miscs.cpp, etc.  The declaration
* should probably go to hw_specific.h, too.
*/

#include <rodos.h>
#include <hal/hal_uart.h>
#include "hw_specific.h"
#include "hw_core.h"
#include "hw_idle.h"

// TODO: move this define someplace else
#define UART_DEBUG UART_IDX0

HAL_UART uart_stdout(UART_DEBUG);

long volatile taskRunning= 0;

void hwInit() {
	uart_stdout.init(HW_CONFIG_UART_BAUD);
}


/**
 * Trampoline to the C linkage function
 *
 * I think this function should get a better prototype, namely:
 *
 *     void *hwInitContext (void *, Thread *);
 *
 * Better even, maybe pass the startup function.
 *
 * Also, it should be extern "C" to make implementation in assembler easier.
 *
 * We're using it as a trampoline to pass it to a function with the proposed
 * interface, implemented in assembler.
 */
long *hwInitContext(long *a, void *b)
{
  return (long*)hwCreateContext ((void*)a, (Thread*)b, threadStartupWrapper);
}


/**
 * Trampoline to the C linkage function.
 *
 * Unfortunately, hwResetAndReboot has C++ linkage and I don't want to use
 * mangled names in assembler.
 */
void hwResetAndReboot()
{
  hwSoftReset();
}

/**
  *  global interrupt disable
  */
void hwDisableInterrupts(){
    #warning "no interrupt disable implemented"
}

/**
  *  global interrupt enable
  */
void hwEnableInterrupts(){
    #warning "no interrupt enable implemented"
}

void sp_partition_yield()
{
  hwIdle(); // sleep a little while until the next interrupts comes in
}

void startIdleThread() { }
