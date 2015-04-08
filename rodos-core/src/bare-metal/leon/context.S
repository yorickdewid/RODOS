/* -*- Mode: Assembler -*- */

/**
 * @file
 * @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Copyright 2008-2010 DLR
 *
 * @brief Context switching, including preemption
 *
 * If you call yield() from kernel code, that request will
 * silently be ignored: the kernel is no thread and cannot
 * be switched.
 *
 * Similarly, a preemption interrupt during kernel code execution
 * will be ignored since the kernel cannot be preempted.  This
 * means that timer interrupts during kernel code execution
 * are ok: this code handles the situation cleanly.
 */

#include "hw_asmdef.h"


//////////////////////////////////////////////////////////////////////
// Trap Handlers
.section HW_SEC_TRAP_TEXT

// Return code for hwTrapSaveContextContinue:
.global hwSaveContextIgnore
hwSaveContextIgnore:
  NOP_WAIT2 // for PSR.ICC
  jmp   %l1
  rett  %l2 // PSR!

.global hwTrapSaveContextContinue
hwTrapSaveContextContinue:
  /* The trap already did:

  Called manually (using TA instruction):

  mov   %l2,  %l1
  rd    %psr, %l0
  b     hwTrapSaveContextContinue
  add   %l2, 4, %l2

  Called from timer interrupt:

  b     hwTrapSaveContextContinue
  rd    %psr, %l0
  nop
  nop

  */
  /* We need our own stack frame to store the floating point stuff etc.,
   * so we first need to flush all windows.  We do that by turning the
   * register wheel with SAVEs, letting the overflow trap do the work.
   *
   * By turning the register wheel, the overflow trap will eventually
   * use the same window that we're using now.  And that step will
   * overwrite precious information, namely *our* old PC and nPC.  So
   * we will start by writing our own data into our future stack
   * frame.  Then, the overflow trap may use our registers happily.
   *
   * See hw_defs.h for the structure of our stack frame.  It is *not*
   * API compliant, i.e., it is not a normal stack frame.  It's a dead
   * end that switchContext will have to drive in reverse to get out
   * of it.  Saving is much more complicated because of the hairy
   * register flush.  With the right setup of CWP and WIM, unwinding
   * it is simple.
   *
   * We'll start off by filling our future stack frame.
   * The current %sp is available in %i6, our future (and current) %fp.
   * Our future %sp will be offset by HW_CTXT_SIZEOF.  We'll never
   * actually set it though, and we also simply return our frame pointer
   * to the scheduler.
   *
   * We already have in registers:
   *    PSR      ->  %l0
   *    old PC   ->  %l1
   *    old nPC  ->  %l2
   *
   * We won't store WIM, because we'll start with a fresh value of CWP
   * and WIM when switching back to this task.
   *
   * We also won't store Lx and Ix, because they belong to this trap
   * itself and may be changed freely (note that we may even be in
   * the invalid window here).
   *
   * Let's call the wheel position before this trap WheelPos 0.
   * So in this trap, we're now at WheelPos -1.  WheelPos 0 cannot be
   * invalid, but WheelPos -1 may very well be marked invalid.
   */

  // if we got here from a software trap, skip the tick increment
  rd    %tbr, %l3
  srl   %l3, TBR_TT_SHIFT, %l3
  and   %l3, TBR_TT_MASK, %l3
  mov   0x80, %l4
  cmp   %l3, %l4
  bge   software_trap
   nop

  // increment 64-bit tick counter
  // by number of reload value +1
  set   hwTickerBase, %l3 
  ldd	[%l3], %l4
  set   hwTickerReload, %l6
  ld    [%l6], %l6
  add   %l6, 1, %l6
  addcc	%l5, %l6, %l5
  addx	%l4, 0, %l4
  std	%l4, [%l3]

software_trap:
   
  /* If we were in supervisor mode, ignore the task switch request.
   * Both preemptive and cooperative switches are rejected in supervisor mode.
   * E.g. if you use puts() in supervisor mode, you'll use busy waiting.
   */
  andcc %l0, HW_BITS(HW_PSR_PS), zero
  bnz,a hwSaveContextIgnore                  // if non-zero, abort switching
  wr    %l0, %psr                            // branch and re-install PSR during delay slot

  //We want to stop saving the context if scheduling is locked
  //Therefore we load the boolean value and look whether it's set or not and branch to hwSaveContextIgnore if it's not.
  sethi %hi(isSchedulingEnabled), %l3		//Loading the address
  ld [%l3 + %lo(isSchedulingEnabled)], %l3
  tst %l3									//Test it
  bz,a hwSaveContextIgnore					//If it's zero, so if isSchedulingEnabled == false branch to hwSaveContextIgnore
  wr %l0, %psr								//and reinstall PSR in delay slot
  
  //We want to stop saving the context if NOW() < timeToTryAgainToSchedule but only if this context switch is timer interrupt induced
  //As we can't call C-Functions from here (especially no functions using save as we are in supervisor mode and a window underflow trap wouldn't be handled) we have
  //  to calculate the value of NOW ourselves. Luckily we can leave out some stuff as this is all happening without an interrupt (hwGetNanoseconds has to take care
  //  of interrupts as it's not running in supervisor mode).
  // This is mostly written by disassembling hwGetNanoseconds and looking how the gcc does it
  
  //If the previous trap was a timer interrupt from timer 1 check whether NOW() < timeToTryAgainToSchedule
  rd %tbr, %l3														//Read TrapBaseRegister. Bits 4 to 11 contain trap number
  srl %l3, 4, %l3													//Shift Bits 4 to 11 to Bits 0 to 7
  and %l3, 0xFF, %l3												//We just want the least significant Byte
  xorcc %l3, 0x18, %g0												//If the result of this operation is non-zero the trap was not trap 0x18 so not timer interrupt induced
  bnz,a .L_notTimerInterruptInduced
  nop
  
  //This calculates the same as hwGetNanoseconds does
  sethi %hi(hwTickerBase), %l3
  ldd [%l3 + %lo(hwTickerBase)], %l4								//Load hwTickerBase
  sethi %hi(HW_BASE_OCR + HW_OCR_TIMER_1_COUNTER), %l3
  ld [%l3 + %lo(HW_BASE_OCR + HW_OCR_TIMER_1_COUNTER)], %l6			//Load Timer 1 Counter
  subcc %l5, %l6, %l5												//Subtract Timer 1 counter from ticker base
  subx %l4, %g0, %l4												//	continue subtracting (double word)
  
  //Now we have the absolute number of ticks. To get the time in nanoseconds we have to multiply by a hardware dependent value
  umul %l5, 1000, %l5												//multiply by (1000000000ULL / HW_CONFIG_TIMER_CLOCK_HZ) to get the time in nanoseconds.
  rd %y, %l6
  smul %l4, 1000, %l4												// continue multiply (double word)
  add %l4, %l6, %l4													//adding most significant word from the first multiplication
  
  //The current time is now in %l4 and %l5
  
  sethi %hi(timeToTryAgainToSchedule), %l3							//Prepare to load timeToTryAgainToSchedule
  ldd [%l3 + %lo(timeToTryAgainToSchedule)], %l6					//Load timeToTryAgainToSchedule
  subcc %l7, %l5, %l5												//Subtract NOW() from timeToTryAgainToSchedule
  subxcc %l6, %l4, %l4
  bpos,a hwSaveContextIgnore										//If timeToTryAgainToSchedule is greater than NOW(), abort context switch
  wr %l0, %psr														//and reinstall PSR in delay slot*/
  
.L_notTimerInterruptInduced:

  mov %y, %l3

  std %l0,  [ %i6 + HW_CTXT_OFFSET_PSR_PC ]  // store %l0 (PSR) and %l1 (PC) at once
  std %l2,  [ %i6 + HW_CTXT_OFFSET_NPC_Y  ]  // store %l2 (NPC) and %l3 (Y)  at once

  st  %g1,  [ %i6 + HW_CTXT_OFFSET_G(1) ]    // don't store %g0 == zero

  std %g2,  [ %i6 + HW_CTXT_OFFSET_G(2) ]
  std %g4,  [ %i6 + HW_CTXT_OFFSET_G(4) ]
  std %g6,  [ %i6 + HW_CTXT_OFFSET_G(6) ]

  /* Our Ix registers are the callee's Ox registers, and since will not be saved by
   * the register flush, so we must save them seperately.
   * Storing %i6 is unnecessary, because it's our stack pointer.
   * It's used for the Y register instead (see hw_defs.h).
   */
  std %i0,  [ %i6 + HW_CTXT_OFFSET_O(0) ]
  std %i2,  [ %i6 + HW_CTXT_OFFSET_O(2) ]
  std %i4,  [ %i6 + HW_CTXT_OFFSET_O(4) ]

  st  %i7,  [ %i6 + HW_CTXT_OFFSET_O(7) ]    // don't store %i6

#if HW_CONFIG_HAVE_FLOAT
  st  %fsr, [ %i6 + HW_CTXT_OFFSET_FSR ]

  .irp reg, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30
    std %f\reg,  [ %i6 + HW_CTXT_OFFSET_F(\reg) ]
  .endr
#endif

  /* Remember the context pointer in a global register as we're about to reset CWP. */
  mov %i6, %g2

  /* For the register flush, we need the window overflow trap.  But unfortunately,
   * we're running with disabled interrupts here, so if we trap, we cause error
   * mode.  So we first prepare the stack, CWP and WIM in such a way that traps
   * are ok, and then switch ET=1 to allow traps (including interrupts!).
   *
   * We don't change PIL, because we can cope with interrupts (we can cope with
   * SAVE traps, so we can cope with interrupts, we hope).  Int 15 cannot be
   * masked with PIL anyway, so why bother.  ET can mask it.  (Why did Sun
   * implement an 'NMI' that can be masked with ET?  Why not simply mask with
   * PIL=15, too?  Strange design.)
   *
   * Note again that we might be in the invalid window now, because we're
   * inside a trap.  So we need one RESTORE first.
   *
   * In itself, this RESTORE won't trap, because we cannot enter a trap from an
   * invalid window and the caller's window is guaranteed to be valid.
   */

  mov    HW_CONFIG_REG_WINDOWS - 1, %g1         // max. N-1 windows need saving (one is invalid)

  rd    %wim, %g4                               // read current WIM
  wr    %g0,  %wim                              // no save/restore traps, please
  NOP_WAIT0  // for WIM

  sethi  CONST_HI(HW_INIT_PSR_SUPERVISOR), %g6  // prepare value for new PSR

.L_RegWindowTryNext:
  rd    %psr, %g3                               // read current CWP
  srl   %g4, %g3, %g5                           // (WIM >> CWP)
  save       // WIM!                            // turn register window wheel
  subcc %g5, 1, zero                            // (WIM >> CWP) == 1 => we were in invalid window
  bne   .L_RegWindowTryNext                     // in that case: start saving
  subcc %g1, 1, %g1                             // decrement window counter

.L_RegWindowFlushNext:                          // from now on, all windows need saving
  SAVE_WINDOW(%sp)                              // we already turned, so directly save
  save                                          // turn further
  bnz   .L_RegWindowFlushNext                   // if there are windows left, then repeat
  subcc %g1, 1, %g1                             // decrement window counter

  // Set supervisor mode from scratch (with fresh CWP):
  wr    %g6, CONST_LO(HW_INIT_PSR_SUPERVISOR), %psr
  NOP_WAIT1        // for PSR

  /* Start by using a clean stack with CWP=0, WIM=2. */

  // Set fresh WIM.  We use WIM=2, meaning that CWP=0 is valid and
  // that the next RESTORE with trap.  There will not be a RESTORE:
  // if schedulerWrapper returns, it will cause an error instead.
  mov  2, %wim
  NOP_WAIT0                                          // for WIM

  /* Load stack frame of scheduler:
   * Why don't we merge this with the boot stack? */
  sethi %hi(__schedulerStackFrame__), %g1            //
  ld    [ %g1 + %lo(__schedulerStackFrame__) ], %fp  // PSR! // __schedulerStackFrame__ is variable

  /* Enter another stack frame so that SAVE+RESTORE doesn't crash (by trying
   * to return to the invalid window we're in now). */
  add   %fp, -HW_FRAME_SIZEOF, %sp

  /* Enable interrupts */
  wr   %g6, CONST_LO(HW_INIT_PSR_SUPERVISOR | HW_BITS(HW_PSR_ET)), %psr
  NOP_WAIT0                                          // for PSR.ET

  /* Call scheduler with different return address */
  call  schedulerWrapper
  mov   %g2, %o0                                     // param 1: context pointer
  

//////////////////////////////////////////////////////////////////////
// Normal Text Section
.section HW_SEC_TEXT

.global hwCreateContext
.type   hwCreateContext,#function
hwCreateContext:
  /* Create a context that looks just like the one that hwTrapSaveContextContinue
   * creates, with a 'return' address to the start function.
   *
   * Our params:
   *
   *   %o0    = stack top pointer
   *   %o1    = thread pointer
   *   %o2    = call target
   *
   * We need to set the registers as follows:
   *
   *   %o0    = thread pointer
   *   %pc    = call target
   *   %npc   = call target + 4
   *   %psr   = user init PSR (see hw_defs.h) but with S=1 and ET=0 (xored with those two bits)
   *
   * We will return the pointer to the stack frame, derived from the stack top pointer.
   *
   * All other registers will be set to 0.
   *
   * Further, we will need another stack frame 'before' the saved one.  It must contain
   * a valid stack pointer.
   *
   * %psr.cwp will be set to 0 in asmSwitchToContext, so there's no need to do that here.
   * %wim will be set to 1 in the same function.
   */

  /* We're a real function, so behave well and set up a stack frame. */
  save %sp, -HW_FRAME_SIZEOF, %sp
  /*
   * %i0  = address of stack top
   * %i1  = address of object
   * %i2  = address of function to call
   */

  /* First enforce SPARC stack alignment, no matter what we get.
   * Stack grows down, so just clear bits. */
  andn %i0, 7, %i0

  /* Now make space for an additional stack frame of 64 bytes (the minimum) */
  sub  %i0, -HW_FRAME_SIZEOF, %l3  // this is a virtual SAVE
                      // I0 is the fp of the additional frame,
                      // L1 is the fp of the context frame (the special dead end frame)

  /* Clear context structure and the additional stack frame */
  mov  %i0, %o1                  // End of area to clear
  call memClear64
  sub  %l3, HW_CTXT_SIZEOF, %o0  // Start of area to clear

  /* Store the additional frame's frame pointer */
  st   %i0, [ %l3 + HW_FRAME_OFFSET_FP   ]

  /* Store object pointer */
  st   %i1, [ %l3 + HW_CTXT_OFFSET_O(0) ]

  /* Store start address */
  st   %i2, [ %l3 + HW_CTXT_OFFSET_PC ]
  add  %i2, 4, %l0
  st   %l0, [ %l3 + HW_CTXT_OFFSET_NPC ]

  /* Store PSR */
  set  HW_INIT_PSR_USER, %l0
  st   %l0, [ %l3 + HW_CTXT_OFFSET_PSR ]

  /* Return the fp of the special frame as context pointer */
  ret
  restore zero, %l3, %o0


.global __asmSaveContextAndCallScheduler
.type   __asmSaveContextAndCallScheduler,#function
__asmSaveContextAndCallScheduler:
  ta    HW_USER_TRAP_SAVE_CONTEXT    // switch to supervisor mode and initiate task switch
                                     // reactivation of this task will continue after this
                                     // TA instructure.

  retl                               // return
  nop


.global __asmSwitchToContext
.type   __asmSwitchToContext,#function
__asmSwitchToContext:
  /* Finalise task switch
   *
   * We arrive here from supervisor mode from the scheduler
   * (or the boot routine).  Interrupts are enabled.
   *
   * Note that the current stack will die and never be
   * used again.  Instead, next time we call the scheduler,
   * it will get a fresh stack.
   */

  /* First secure the context pointer to a global register,
   * as we're about to reset CWP. */
  mov   %o0, %g1

  /* We're coming from scheduler mode here, which has ET=1, PIL=0, S=1. */

  /* Set trap mode: ET=0, reset CWP to 0. restore ICC from thread later. */
  sethi CONST_HI(HW_INIT_PSR_SUPERVISOR), %l0
  wr    %l0, CONST_LO(HW_INIT_PSR_SUPERVISOR), %psr   // %L0 will continue to contain old PSR.
  NOP_WAIT0  // for PSR.CWP

  /* Set initial WIM.
   * We're using WIM=4 (single invalid window is 2) here
   * while CWP points to window 0.  This is done because
   * the RESTORE that RETT implements does not cause a
   * window underflow trap, but enters error mode instead.
   * Therefore, we'll restore window 1, the one we return to,
   * manually, and window 2 is the first illegal one, thus WIM=2^2.
   */
  mov   4, %wim
  NOP_WAIT0  // for WIM

  /* The first few restores we do via G1, because PSR.CWP needs to
   * stabilise.  Then we load G1 to its final position, I6, our
   * frame pointer, the callers stack pointer.  The rest will be
   * restored via I6. */

  /* Start restoring: */
  ldd  [ %g1 + HW_CTXT_OFFSET_G(2) ], %g2
  ldd  [ %g1 + HW_CTXT_OFFSET_G(4) ], %g4
  ldd  [ %g1 + HW_CTXT_OFFSET_G(6) ], %g6

  /* move stack pointer to final position */
  mov %g1, %i6                                // PSR.CWP!

  /* finally also restore G1 */
  ld   [ %g1 + HW_CTXT_OFFSET_G(1) ], %g1

  /* Now load via I6 just like SaveContext: */

  /* Our Ix registers are the callee's Ox registers, and since will not be saved by
   * the register flush, so we must save them seperately.
   * Storing %i6 is unnecessary, because it's our stack pointer.
   * It's used for the Y register instead (see hw_defs.h).
   */
  ld   [ %i6 + HW_CTXT_OFFSET_O(7) ], %i7

  ldd  [ %i6 + HW_CTXT_OFFSET_O(0) ], %i0
  ldd  [ %i6 + HW_CTXT_OFFSET_O(2) ], %i2
  ldd  [ %i6 + HW_CTXT_OFFSET_O(4) ], %i4

#if HW_CONFIG_HAVE_FLOAT
  .irp reg, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30
    ldd  [ %i6 + HW_CTXT_OFFSET_F(\reg) ], %f\reg
  .endr

  ld   [ %i6 + HW_CTXT_OFFSET_FSR  ], %fsr
#endif

  ldd  [ %i6 + HW_CTXT_OFFSET_NPC_Y ], %l2    // old NPC (%l2) and Y (%l3)
  wr   %l3, %y
  NOP_WAIT0  // for Y

  /* Now we need to RESTORE the register window we'll be
   * returning to.  The Ox registers are already in place (they are
   * called Ix in our window).
   *
   * Unfortunately, we cannot force the RETT to cause a window
   * underflow trap, so we'll just do it ourselves.
   *
   * Neither RESTORE nor SAVE in the following paragraph will
   * trap (because CWP=0 and WIM=4).
   */
  restore
    LOAD_WINDOW(%sp)
  save

  RESET_WATCHDOG(%l4,%l5)

  /* Finally, write PSR again in case the condition codes changed.
   * They shouldn't change, but better safe than sorry -- condition
   * codes change easily if anything is touched here.
   */
  ldd   [ %i6 + HW_CTXT_OFFSET_PSR_PC ], %l0          // old PSR (%l0) and PC (%l1)

  set   HW_PSR_SWITCH_MASK, %l4
  and   %l0, %l4, %l4                                 // cut out old user bits

  sethi CONST_HI(HW_INIT_PSR_SUPERVISOR), %l0
  or    %l0, %l4, %l0                                 // insert old user bits

  wr    %l0, CONST_LO(HW_INIT_PSR_SUPERVISOR), %psr
  NOP_WAIT2  // for PSR.ICC: must be stable when reaching the return target

  /* return as if returning from interrupt */
  jmp   %l1
  rett  %l2   // PSR!


//////////////////////////////////////////////////////////////////////
// DATA
.section HW_SEC_ROM_DATA

hwPanicStartReturnedMsg:
  .asciz "Start routine returned."

