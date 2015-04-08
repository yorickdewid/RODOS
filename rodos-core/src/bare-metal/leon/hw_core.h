/* -*- Mode: C++ -*- */

/*********************************************************** Copyright
 **
 ** Copyright (c) 2010, German Aerospace Center (DLR)
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/

/**
 * @file
 * @date 2010/08/02
 * @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Copyright 2010 DLR
 *
 * @brief Hardware specifice definitions for C
 */

#ifndef RODOS_HW_LEON2_CORE_H
#define RODOS_HW_LEON2_CORE_H

#include <thread.h>
#include "hw_defs.h"
#include "hw_uart.h"

/**
 * Type for a pair of OCR register value and its content at boot up */
struct HWConfigRegVal {
  unsigned volatile *address;
  unsigned value;
} __ALIGN8;  // so we can load the entries with LDD


/**
 * Type of constructor functions in the __ctors area. */
typedef void (*HWConstructor)();


/**
 * Access on-chip register */
#define HW_OCR(_number) ((unsigned volatile *)(HW_BASE_OCR + (_number)))


extern "C" {
  /**
   * The following variables' addresses are set by the linker script: */
  /**@{*/
  extern unsigned long long __textROMStart;       ///< start of code in ROM
  extern unsigned long long __textStart;          ///< start of code at final location
  extern unsigned long long __textEnd;            ///< end of code at final location

  extern HWConstructor      __ctorsStart;
  extern HWConstructor      __ctorsEnd;

  extern unsigned long long __dataROMStart;
  extern unsigned long long __dataStart;
  extern unsigned long long __dataEnd;

  extern unsigned long long __RODataStart;        ///< read-only part of __dataStart...__dataEnd
  extern unsigned long long __RODataEnd;

  extern unsigned long long __writeProtectStart;  ///< area containing all code plus .rodata
  extern unsigned long long __writeProtectEnd;

  extern unsigned long long __SDRAMDataROMStart;
  extern unsigned long long __SDRAMDataStart;
  extern unsigned long long __SDRAMDataEnd;

  extern unsigned long long __bssStart;
  extern unsigned long long __bssEnd;

  extern unsigned long long __stackStart;
  extern unsigned long long __stackEnd;
  /**@}*/


  /**
   * The following are initial values for several hardware registers
   *
   * These are defined in hw_config.cpp.  Some values can be influenced by
   * changing hw_config.h (or by using -D on the compiler's command line).
   */
  /**@{*/
  extern unsigned const hwInitMCFG1;                ///< MCFG1 will be modified during reset

  extern HWConfigRegVal const hwInitConfigRegVal[]; ///< other registers with init values

#if HW_CONFIG_HAVE_WATCHDOG
  extern unsigned const hwResetWatchdog;            ///< To reset watchdog in __asmSwitchToContext
#endif
  /**@}*/

  extern unsigned long long volatile hwTickerBase; ///< Each ticker2 interrupt increments this

  /**
   * Config stuff the hwResetContinue function sets:
   */
  extern unsigned hwConfigArch;           /**< Architecture (see HW_ARCH_* constants) */


  /**
   * hwInSysPanic will be -1 if we're in system panic mode, 0 before that.
   *
   * It is used as a recursion protection against nested system
   * panicking.  Instead of an infinite loop, we'll enter error mode.
   */
  extern char hwInSysPanic;


  /**
   * The main routine, which will be invoked as soon as we're ready to go.
   * For some reason, rodos.h does not declare it.
   *
   * If main returns, we're doomed => hwSysPanic().
   */

  int main (int, char **) __attribute__((__noreturn__));


  /**
   * Software reset.
   * Does essentially exactly the same as a power-up.
   *
   * This function may be invoked from both supervisor or user mode.
   */
  void hwSoftReset() __attribute__((__noreturn__));


  /**
   * Enter error mode.
   *
   * This function may be invoked from both supervisor or user mode.
   */
  void hwErrorMode() __attribute__((__noreturn__));


  /**
   * hwResetContinue is called after the reset handler initialised a C environment.
   *
   * This is the first routine programmed in C that is called after reset.
   * The hwTrapReset and hwTrapResetContinue have initialed some registers in
   * such a way that invoking a C function is possible.  E.g. there will be
   * a valid stack register.
   *
   * Note that this function still runs in supervisor mode with interrupts disabled!
   * With ET=0, any trap (e.g. a register window overflow) will cause error mode.
   * So you better be fast to get the interrupts running and setting ET to 1
   * (use hwEnableTraps).
   *
   * This function must only be invoked in supervisor mode.
   */
  void hwResetContinue(unsigned int powerOnPSR, unsigned int powerOnWIM)
    __attribute__((__noreturn__));


  /**
   * Function to create an initial context on the stack. */
  void *hwCreateContext (void *stack_top, Thread *object, void (*start)(Thread *));
}

#endif /* RODOS_HW_LEON2_CORE_H */

