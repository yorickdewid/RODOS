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
 * @brief Trap handlers for C code
 *
 * This module makes it easily possible to install trap handlers
 * that are written in C.  The SPARC architecture isn't really
 * cooperative with this -- all the trap low-level handlers must
 * be written in assebler due to the different calling conventions
 * and restrictions.  This module provides a trampoline.  This way,
 * additional hardware drivers can be written entirely in C.
 *
 * To install a trap handler, just write a function with an appropriate
 * name.  Instead of a dynamic table, this implementation uses weak
 * symbols and lets the linker do the job at compile time.  This way, no
 * memory is wasted for a secondary trap table.
 *
 * To define a trap handler, e.g. for system trap 0x08, implement  a
 * C function of the following kind:
 *
 *    extern "C" void hwTrapHandler0x08(void *)
 *
 * System traps have get the trapped PC as input and have no output
 * parameter.
 *
 * During trap handling, interrupts are switched off using the PIL
 * field in the PSR, meaning that interrupt 15 can still
 * interrupt.  Traps are enabled (ET=1) during trap handler execution
 * (that's necessary for executing arbitrary C code).
 *
 * Hexadecimal digits are written in capital letters in the trap
 * handler name.
 *
 * Also note that some traps cannot be overloaded, because they are
 * part of the system itself:
 * - register window overflow/underflow (Traps 0x05 and 0x06),
 * - preemption interrupt (Trap 0x18 = Timer 1, Int 8),
 */

#ifndef RODOS_HW_LEON2_TRAP_H
#define RODOS_HW_LEON2_TRAP_H

#include "hw_defs.h"

/**
 * Structure for storing the floating point state */
struct HWFloatState {
    unsigned freg[32];
    unsigned fsr;
} __ALIGN8;


extern "C" {
  /**
   * Save the floating point state at the beginning of a trap that uses
   * floating point. */
  void hwFloatSave(HWFloatState &floatState);

  /**
   * Restore a floating point state at the end of a trap that uses
   * floating point. */
  void hwFloatRestore(HWFloatState const &floatState);

  /**
   * Ensure supervisor mode and possibly increase PIL to maximum
   * in order to switch off interrupts.  Note that you cannot switch
   * off traps completely.  You don't want that, anyway, because
   * you cannot really execute C code then.
   *
   * By default, this switches off interrupts (except for INT 15).
   * To keep interrupts running, pass 'true' as the first argument.
   *
   * As a side effect, it returns a value of the PSR suitable
   * for use with hwLeaveTrapMode().
   *
   * This is usually followed by hwLeavTrapMode(), which takes
   * the value returned by hwEnterTrapMode() as an input.
   *
   * Note: if you nest calls to this function, you cannot switch
   *       back on interrupts, but only disable them.
   */
  extern inline unsigned hwEnterTrapMode ()
  {
    register unsigned ret __asm__("o0");
    __asm__ __volatile__ (
          "ta %1"
      : "=r" (ret)
      : "I" (HW_USER_TRAP_ENTER_TRAP_MODE)
    );
    return ret;
  }

  /**
   * Enable traps and return to supervisor/user mode before the
   * last hwEnterTrapMode().
   */
  extern inline void hwLeaveTrapMode (unsigned oldPSR)
  {
    register unsigned old __asm__("o0")= oldPSR;
    __asm__ __volatile__ (
          "ta %0"
      :
      : "I" (HW_USER_TRAP_LEAVE_TRAP_MODE), "r" (old)
    );
  }


  /**
   * Enable interrupts in the primary interrupt controller.
   *
   * Takes a bit mask to select the interrupts.
   */
  void hwEnableInt(unsigned whichMask);


  /**
   * Disable interrupts in the primary interrupt controller.
   *
   * Takes a bit mask to select the interrupts.
   */
  void hwDisableInt(unsigned whichMask);


  /**
   * Clear pending bits of interrupts in the primary interrupt controller.
   *
   * Takes a bit mask to select the interrupts.
   */
  void hwClearInt(unsigned whichMask);
}

#endif /* RODOS_HW_LEON2_TRAP_H */
