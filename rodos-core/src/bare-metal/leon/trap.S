/**
 * @file
 * @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Copyright 2008-2010 DLR
 *
 * @brief Handler for arbitrary traps to enter C code.
 *
 * To be able to call C code in a trap is really complex on a Sparc,
 * because of the register window handling that does not allow a trap
 * to easily save and restore a register window.  The following code
 * shows this.  Much work that could easily be done with the register
 * overflow and underflow traps must be programmed again here, because
 * we cannot simply invoke those traps here.
 *
 * In total, this code is almost as complicated as a task switch.
 */

#include "hw_asmdef.h"

.section HW_SEC_TRAP_TEXT

.global hwSysTrapTrampoline
hwSysTrapTrampoline:
  /* The trap handler already did:

  rd    %psr, %l0
  sethi %hi(hwTrapHandler##_N), %l4
  b     hwTrapTrampoline
  or    %l4, %lo(hwTrapHandler##_N), %l4

  */

  // If we're in the invalid window, then make it valid manually.  We cannot
  // enable traps until then, and we cannot use the normal window overflow
  // traps here, unfortunately, because it would mean we need to go back to
  // the caller's reg window and enable traps there, which we cannot do
  // because a trap then would overwrite our local registers.
  rd    %wim, %l3
  rd    %y,   %l6                       // Save Y register in L6

  // Test whether we're in the invalid window here
  srl   %l3, %l0, %l7                   // CWP are the least significant five bits of PSR, so this
                                        // shifts WIM by CWP to right
  subcc %l7, 1, zero                    // if (WIM >> CWP) == 1, then we're in the invalid regwin

  bne   .L_ValidRegisterWindow
  mov   %g1, %l7                        // Save g1 to a local register.

  // We're in the invalid window.  We cannot trap here, unfortunately, so this
  // code is copied from regwin.S.
  srl   %l3, 1, %g1
  sll   %l3, HW_CONFIG_REG_WINDOWS - 1, %l3

  save  %l3, %g1, %g1
    wr  %g1, %wim
    NOP_WAIT0
    SAVE_WINDOW(%sp)
  restore

  // We can use %o registers (as temporaries) now without overwriting
  // someone else's register window.  We'll do that to make
  // a stack frame.
.L_ValidRegisterWindow:

  // We're in a system trap, so switch off interrupts by setting PIL to current PIL,
  // stored in L5 by the trap handler.  Keep the current L0 unmodified, because we
  // use the old PIL later when switching back.
  //
  // PIL must be at least at high as the preemption interrupt's priority, or we might
  // lose preemptions (the preeption code will only switch when in user mode, so
  // if we're is superviser mode for too long, the watchdog may hit us)!
  or    %l0, HW_BITS(HW_PSR_PIL), %l5         // PIL => max.
  wr    %l5, %psr                             // new PIL
  NOP_WAIT0


  add   %fp, -TRAP_FRAME_SIZEOF, %sp          // immitate a normal function entry (SAVE was already
                                              // done by trap invocation).
  // we have to wait the full write cycle after changing PIL before we're allowed to
  // set ET=1, because some SPARCs generate spurious (wrong) interrupts otherwise.
  // Dunno whether LEON is among them, but better safe than sorry.

  // We need to save all %g registers because they might be used in C code:
  std   %l6, [ FRAME_OFFSET_G(0) ]            // %y was in l6, %g1 was saved in %l7.
                                              // Now store both in memory.
  std   %g2, [ FRAME_OFFSET_G(2) ]            // start saving global registers

  // Floating point registers must be saved manually.
  wr    %l5, HW_BITS(HW_PSR_ET), %psr         // enable traps
  NOP_WAIT0           // for PSR: when we're in trap handler code, interrupts must be enabled

  std   %g4, [ FRAME_OFFSET_G(4) ]            // continue saving

  // From here on, PSR.CWP and WIM might change (some day) if the kernel
  // is preempted.  It won't happen now, but we still don't access the old
  // PSR and WIM register values just to be sure.  If we need them, we'll get
  // fresh values from the registers.

  // adjust the return address in the delay slot of the call:
  mov   %l1, %o0  // return address of call
  jmpl  %l4, %o7  // PSR! (at call target)
  std   %g6, [ FRAME_OFFSET_G(6) ]            // continue saving

  // First restore PSR and disable traps again.
  // Using the user trap to disable interrupts is a clean way to
  // even handle preemptible kernels one day.
  ta HW_USER_TRAP_DISABLE_TRAPS               // disable traps

  // Fresh values for PSR.CWP and WIM:
  rd    %wim, %g6                             // get fresh value of WIM
  wr    %g0, %wim                             // switch off window traps: we're about to RESTORE
  NOP_WAIT0

  ldd   [ FRAME_OFFSET_G(0) ], %g2            // start restoring globals: %g2/%g3 == old %y/%g1
  wr    %g2, %y

  // From L0 we'll reinstall (at least) old PIL, S, PS, ICC, et.al.:
  andn  %l0, HW_BITS(HW_PSR_CWP), %g5         // L0 now contains old PSR, except for CWP

  // RETT will fail if it turns the reg window wheel into an invalid window.
  // There is no way to get around this, e.g. by a RESTORE before switching
  // off the interrupts, because as soon as we do restore, our local registers
  // may be overwritten by an incoming trap (and we cannot mask all interrupts
  // with PIL, but only with ET).  So unfortunately, we may need to restore
  // one window manually.

  restore                                     // go to window that RETT will enter; traps are off

  rd    %psr, %g4                             // read PSR (we only need CWP)
  and   %g4, HW_BITS(HW_PSR_CWP), %g4         // get CWP

  srl   %g6, %g4, %g7
  subcc %g7, 1, zero                          // (WIM >> CWP) == 1 => we're in invalid window
  bne   .L_PrevRegisterWindowIsValid
  mov   zero, %g7                             // if not, no change to WIM

  sll   %g6, 1, %g7                           // rotate WIM by one bit
  srl   %g6, HW_CONFIG_REG_WINDOWS - 1, %g6

  LOAD_WINDOW(%sp)                            // restore window

.L_PrevRegisterWindowIsValid:
  // join old ICC, PS, PIL with possibly new CWP into PSR
  wr    %g4, %g5, %psr
  NOP_WAIT2 // for PSR

  mov   %g3, %g1

  save      // PSR.CWP!                       // go back to trap window
  wr    %g6, %g7, %wim                        // store new WIM
  NOP_WAIT0 // for WIM

  // Now restore the other global registers:
  ldd   [ FRAME_OFFSET_G(2) ], %g2
  ldd   [ FRAME_OFFSET_G(4) ], %g4
  ldd   [ FRAME_OFFSET_G(6) ], %g6

  // Return:
  !jmp   %l1
  !rett  %l2 // PSR! WIM! (but old WIM (which is 0) is also ok)
  !
  ! Instead of returning to the previous thread we want to jump to the
  ! schedulerWrapper from here. We can skip the restore of the rett
  ! instruction because hwTrapSaveContextContinue expects the register
  ! wheel to be rotated upon entry, just as it is at this point.
  ! Make sure the register file is coherent with a trap entry and
  ! move PSR to %l0 again. -AndreB
  !
  rd    %psr, %l0
  sethi %hi(hwTrapSaveContextContinue), %l3
  or    %l3, %lo(hwTrapSaveContextContinue), %l3
  jmpl  %l3, %g0
  nop                                         // delay slot


//////////////////////////////////////////////////////////////////////
// Normal text section:
.section HW_SEC_TEXT

.global hwFloatSave
.type   hwFloatSave, #function
hwFloatSave:
  .irp  reg, 0,2,4,6,8, 10,12,14,16,18, 20,22,24,26,28, 30
  std   %f\reg, [ %o0 + 4 * \reg ]
  .endr
  retl
  st    %fsr, [ %o0 + 4 * 32 ]


.global hwFloatRestore
.type   hwFloatRestore, #function
hwFloatRestore:
  .irp  reg, 0,2,4,6,8, 10,12,14,16,18, 20,22,24,26,28, 30
  ldd   [ %o0 + 4 * \reg ], %f\reg
  .endr
  retl
  ld    [ %o0 + 4 * 32 ], %fsr
