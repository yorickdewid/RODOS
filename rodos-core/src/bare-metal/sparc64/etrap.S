/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Prepare the trap entry.
 */

#include <spitfire.h>
#include <pstate.h>
#include <asi.h>
#include <param.h>
#include <trap.h>

  .section    .text
  .align      4
  .register   %g2, #scratch
  .register   %g3, #scratch

/*
 * Configuration on entry:
 *    - %g7 holds the %pc so we can leave this with jmpl %g7 + 0x4
 *
 * Temporary registers are: AG %g1-%g6
 */
  .global     etrap, etrap_irq, etrap_tl1
etrap_tl1:
  /* force trap level 1 */
  wrpr        %g0, 1, %tl
  wrpr        %g0, (PSTATE_PRIV|PSTATE_AG), %pstate

etrap:
etrap_irq:
  /* TSTATE contains the state from the previous trap level, comprising the
   * contents of the CCR, ASI, CWP, and PSTATE registers. Therefore we have
   * to save it here and restore it later.
   */
  rdpr        %tstate, %g1

  /* if we are already in supervisor mode, skip window and ctx change */
  /* update cwp from TL[0] if a nested trap messed it up */
  and         %g1, TSTATE_CWP, %g2
  andcc       %g1, TSTATE_PRIV, %g0
  bne,pn      %xcc, etrap_kernel
    wrpr        %g2, %g0, %cwp

etrap_user:
  /* Trap from user mode */
  /* prevent cleanwindow trap */
  wrpr        %g0, (NWINDOWS-1), %cleanwin

  /* get window on thread's stack */
  save        %sp, -TRAP_FRAME_SZ, %sp

  /* mark canrestore as other */
  rdpr        %canrestore, %g2
  wrpr        %g2, %otherwin
  wrpr        %g0, %canrestore
  wrpr        %g0, (WSTATE_O(2)|WSTATE_N(2)), %wstate

  /* switch context primary: 0 */
  mov         PRIMARY_CONTEXT, %g2
  stxa        %g0, [%g2] ASI_DMMU
  membar      #Sync

  ba          etrap_window
    nop

etrap_kernel:
  /* if we already use the kernel stack, just get a window */
  save        %sp, -TRAP_FRAME_SZ, %sp
etrap_window:

  /* save trap state and y (even if deprecated, the compiler may use it) */
  rdpr        %tpc, %g2
  stx         %g1, [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TSTATE]
  rdpr        %tnpc, %g1
  stx         %g2, [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TPC]
  rd          %y, %g2
  stx         %g1, [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TNPC]
  stx         %g2, [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_Y]

  /* save return address in %l0 (current window before we change globals) */
  mov         %g7, %l0

  /* preserve the normal globals in the current window */
  wrpr        %g0, (PSTATE_PRIV|PSTATE_PEF), %pstate
  mov         %g7, %l7
  mov         %g6, %l6
  mov         %g5, %l5
  mov         %g4, %l4
  mov         %g3, %l3
  mov         %g2, %l2
  mov         %g1, %l1

  jmpl        %l0 + 0x4, %g0
    wrpr        %g0, %g0, %tl



/*
 * Configuration on entry:
 *    - %o0-%o5 syscall arguments
 *    - %g7 holds the %pc so we can leave this with jmpl %g7 + 0x4
 *
 * Temporary registers are: AG %g1-%g6
 */
  .global     etrap_sc
etrap_sc:
  /* when we get here we are in TL=1 because of the different priorities of
   * traps and interrupts we do not have to care about %pil here.
   */

  rdpr        %tstate, %g1

  /* update cwp from TL[0] if a nested trap messed it up */
  and         %g1, TSTATE_CWP, %g2
  wrpr        %g2, %g0, %cwp

  /* prevent cleanwindow trap */
  wrpr        %g0, (NWINDOWS-1), %cleanwin

  /* get window on thread's stack */
  save        %sp, -TRAP_FRAME_SZ, %sp

  /* copy system call arguments to the new window */
  mov         %i5, %o5
  mov         %i4, %o4
  mov         %i3, %o3
  mov         %i2, %o2
  mov         %i1, %o1
  mov         %i0, %o0

  /* mark canrestore as other */
  rdpr        %canrestore, %g2
  wrpr        %g2, %otherwin
  wrpr        %g0, %canrestore
  wrpr        %g0, (WSTATE_O(2)|WSTATE_N(2)), %wstate

  /* switch context primary: 0 */
  mov         PRIMARY_CONTEXT, %g2
  stxa        %g0, [%g2] ASI_DMMU
  membar      #Sync

  /* save trap state and y (even if deprecated, the compiler may use it) */
  rdpr        %tpc, %g2
  stx         %g1, [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TSTATE]
  rdpr        %tnpc, %g1
  stx         %g2, [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TPC]
  rd          %y, %g2
  stx         %g1, [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TNPC]
  stx         %g2, [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_Y]

  /* save return address in %l0 (current window before we change globals) */
  mov         %g7, %l0

  /* preserve the normal globals in the current window */
  wrpr        %g0, (PSTATE_PRIV|PSTATE_PEF), %pstate
  mov         %g7, %l7
  mov         %g6, %l6
  mov         %g5, %l5
  mov         %g4, %l4
  mov         %g3, %l3
  mov         %g2, %l2
  mov         %g1, %l1

  /* jump back to the ttable handler where we call the appropriate routine;
   * while doing so, we can also enable interrupts again.
   */
  jmpl        %l0 + 0x4, %g0
    wrpr        %g0, %g0, %tl

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
