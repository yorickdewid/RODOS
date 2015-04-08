/* -*- Mode: Assembler */

/**
 * @file
 * @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Copyright 2008-2010 DLR
 *
 * @brief System panic: send a message via UART, then enter error mode.
 *
 * It's futile trying to put this into ROM, because the UART
 * code is not in ROM (and we don't always want it there), so
 * if RAM access is broken, clean system panic wouldn't work
 * anyway.  Thus, this goes to the normal text section instead
 * of boot.
 */

#include "hw_asmdef.h"

.section HW_SEC_TEXT

.global hwSysPanic
.type   hwSysPanic,#function
hwSysPanic:
  // we get here from bad traps

  // disable traps, raise PIL=max, don't touch CWP
  rd    %psr, %l5
  set   PSR_PIL, %l4
  or    %l4, %l5, %l4
  andn  %l4, PSR_ET, %l4
  wr    %l4, %psr

  // restore old globals
  ldd   [FRAME_OFFSET_G(0)], %g2
  //wr    %g2, 0, %y
  mov   %g3, %g1
  ldd   [FRAME_OFFSET_G(2)], %g2
  ldd   [FRAME_OFFSET_G(4)], %g4
  ldd   [FRAME_OFFSET_G(6)], %g6

  // save away the trap frame
  // %l0 psr
  // %l1 pc
  // %l2 ncp
  // window: %i0-7, %g1-7
  // = 18
  set   ptreg_storage, %l4
  st    %l0, [%l4 + TF_L0]
  st    %l1, [%l4 + TF_L1]
  st    %l2, [%l4 + TF_L2]
  st    %g1, [%l4 + TF_G1]
  std   %g2, [%l4 + TF_G2]
  std   %g4, [%l4 + TF_G4]
  std   %g6, [%l4 + TF_G6]
  std   %i0, [%l4 + TF_I0]
  std   %i2, [%l4 + TF_I2]
  std   %i4, [%l4 + TF_I4]
  std   %i6, [%l4 + TF_I6]
  rd    %tbr, %g1
  st    %g1, [%l4 + TF_TBR]
  st    %o7, [%l4 + TF_RET]

  // we now want to rotate back into the window at wich the trap
  // occurred, but we cannot trap here. Check if we rotate into
  // the invalid window, if so, fixup WIM first.
  mov   2, %g1
  sll   %g1, %l0, %g1
  srl   %g1, 8, %g2
  or    %g1, %g2, %g1
  rd    %wim, %g2
  cmp   %g2, %g1
  be    1f
   sll  %g2, 1, %g1

  // we return into the invalid window

  // calculate new WIM
  srl   %g2, 7, %g2             ! 7 = NWINDOWS-1
  or    %g1, %g2, %g1
  and   %g1, 0xff, %g1

  wr    %g1, 0, %wim

  // DON'T touch %lX or %ix between restore and save

  restore %g0, %g0, %g0
  // load previously saved window
  ldd   [%sp + HW_FRAME_OFFSET_L(0)], %l0
  ldd   [%sp + HW_FRAME_OFFSET_L(2)], %l2
  ldd   [%sp + HW_FRAME_OFFSET_L(4)], %l4
  ldd   [%sp + HW_FRAME_OFFSET_L(6)], %l6
  ldd   [%sp + HW_FRAME_OFFSET_I(0)], %i0
  ldd   [%sp + HW_FRAME_OFFSET_I(2)], %i2
  ldd   [%sp + HW_FRAME_OFFSET_I(4)], %i4
  ldd   [%sp + HW_FRAME_OFFSET_I(6)], %i6
  b     2f
   nop
1:
  // we get here if we can do a restore without running into a trap

  // rotate back into the window at wich the trap occurred
  restore %g0, %g0, %g0
2:
  // save the window in which the trap occurred: %l0-7, %i0-7
  // = 16
  set   ptreg_storage, %g1
  std   %l0, [%g1 + SF_L0]
  std   %l2, [%g1 + SF_L2]
  std   %l4, [%g1 + SF_L4]
  std   %l6, [%g1 + SF_L6]
  std   %i0, [%g1 + SF_I0]
  std   %i2, [%g1 + SF_I2]
  std   %i4, [%g1 + SF_I4]
  std   %i6, [%g1 + SF_I6]

  // now everything is saved, rotate back to avoid losing information

  // we can safely rotate back here since we already made sure we
  // do not rotate into the invalid window. we simply revert the
  // previous restore.
  save  %g0, %g0, %g0

  // we are in the trap window again, don't touch %l0-l2

  // enable traps again
  rd    %psr, %g5
  wr    %g5, PSR_ET, %psr

  // then pass a pointer to a C-struct to the failure handler
  // make sure not to lose the return address
  set   ptreg_storage, %o0
  call  hwHandleSysPanic
   nop

  // if the error handler returns, try to continue

  // in case of software traps, we have to continue
  // *after* the trap instruction (no retry!)
  set   ptreg_storage, %l4
  ld    [%l4 + TF_TBR], %l5
  srl   %l5, TBR_TT_SHIFT, %l5
  and   %l5, TBR_TT_MASK, %l5
  mov   0x80, %l6
  cmp   %l5, %l6
  bl    no_software_trap
   nop

  // fixup pc and npc for software traps
  mov   %l2, %l1
  add   %l2, 0x4, %l2

no_software_trap:

  // then go back to the trampoline handler that does the
  // cleanup. Load the return address of this handler from
  // our saved register file since it might be lost by now.
  ld    [%l4 + TF_RET], %l4
  jmpl  %l4 + 0x8, %g0
   nop



.global hwSoftReset
.type   hwSoftReset,#function
hwSoftReset:
  // initialise PSR.  Note: traps may still be on, so disable them.
  sethi CONST_HI(HW_INIT_PSR_SUPERVISOR), %g4
  wr    %g4, CONST_LO(HW_INIT_PSR_SUPERVISOR), %psr
  NOP_WAIT3        // for PSR
  call  hwStartUp  // do the reset
  nop


.section .bss
.align   8
// we reserve some space for our error report
ptreg_storage:
.skip    40*4

