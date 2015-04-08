/* -*- Mode: C++ -*- */

/**
* @file
* @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2010 DLR
*
* @brief Implementation of everything that hw_specific.h expects.
*
* This also implements stuff in private extern declarations like
* taskRunning, which is found in miscs.cpp, etc.  The declaration
* should probably go to hw_specific.h, too.
*/

#include <rodos.h>
#include "hw_core.h"
#include "hw_idle.h"
#include "hw_trap.h"


extern int fflush(FILE *f);

unsigned hwConfigArch;
char hwInSysPanic= 0;


static unsigned countSetBits (unsigned x) // from Hacker's Delight (Henry S. Warren, Jr.)
{
  x-=  (x >> 1) & 0x55555555U;
  x=   (x & 0x33333333U) + ((x >> 2) & 0x33333333U);
  x=   (x + (x >> 4)) & 0x0f0f0f0fU;
  x+=  x >> 8;
  x+=  x >> 16;
  return (unsigned char)x;
}


void hwResetContinue(unsigned int oldPSR, unsigned int powerOnWIM)
{
  /* The PSR contains version and vendor ID */
  hwConfigArch= HW_EXTRACT(HW_PSR_ARCH, oldPSR);

#if HW_CONFIG_SANITY_CHECKS
  /* ** Some sanity checks ** */

  /* According to the SPARC specs, WIM, when written with ~0, will
   * set only those bits to 1 for which there's a valid register.
   * This works for both LEON2 (NWINDOWS in config reg) and
   * LEON3 (NWINDOWS in asr17).  In fact, it should work for any SPARC.
   */
  unsigned nWindows= countSetBits (powerOnWIM);

  /* Run checks */
  static unsigned volatile test= 0;
  if (! (test == 0)) {
      xprintf(".bss not initialised with %x", test);
      hwErrorMode();
  }

  test= 0xaaff5500;
  if (! (test == 0xaaff5500)) {
      xprintf("Cannot read/write data (RAM or cache misconfigured?) %x", test);
      hwErrorMode();
  }

  if (! (nWindows >= 2 && nWindows <= 32)) {
      xprintf("Illegal number of register windows %d", nWindows);
      hwErrorMode();
  }

#if HW_CONFIG_CHECK_ARCH
  if (! (hwConfigArch == HW_CONFIG_ARCH)) {
      xprintf("Not running on selected architecture %x", hwConfigArch + (HW_CONFIG_ARCH << 16));
      hwErrorMode();
  }
#endif

  if (! (nWindows == HW_CONFIG_REG_WINDOWS)) {
      xprintf("Number of register windows mismatch config setting %d", nWindows);
      hwErrorMode();
  }
#endif

  /* ** Initialise Other Hardware here **
   * (hwInit() is too late: the constructors may already use the hardware). */

  /* ... currently, initialisation is so trivial, it's all done in hw_config.cpp + reset.S ...*/

  /* Run global constructors:
   * Note: These run in supervisor mode with disabled interrupts
   *       (but enabled traps), because we don't know whether it's
   *       a supervisor or a thread initialisation.
   */
  for (HWConstructor *c= &__ctorsStart; c < &__ctorsEnd; c++)
      (*c)();

  /* We can enable interrupts now: everything is prepared to run now.
   * We want S=1, PIL=0. */
  hwLeaveTrapMode(HW_BITS(HW_PSR_PS));

  /* Invoke main: */
  static char const *argv[]= { "-rodos", NULL };
  main (1, (char**)argv);
}

