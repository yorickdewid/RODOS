/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Prepare for return from trap.
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

  .global     rtrap, rtrap_irq, rtrap_tl1
rtrap_irq:
rtrap:
rtrap_tl1:
  /* raise tl, set normal globals and disable interrupts */
  wrpr        %g0, 1, %tl
  wrpr        %g0, PSTATE_PRIV, %pstate

  /* clear the priority interrupt level */
  wrpr        %g0, %g0, %pil

  /* before switching to alternate globals, restore the old ones */
  mov         %l7, %g7
  mov         %l6, %g6
  mov         %l5, %g5
  mov         %l4, %g4
  mov         %l3, %g3
  mov         %l2, %g2
  mov         %l1, %g1

  /* switch to alternate globals and restore tstate with preserved PEF */
  rdpr        %pstate, %l0
  wrpr        %g0, (PSTATE_PRIV|PSTATE_AG), %pstate

  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TSTATE], %g1
  andn        %g1, TSTATE_CWP, %g7
  and         %l0, PSTATE_PEF, %l0
  sllx        %l0, TSTATE_SHIFT_PSTATE, %l0
  sethi       %hi(TSTATE_PEF), %g3
  andn        %g7, %g3, %g1
  or          %g1, %l0, %g1

  /* restore tpc, tnpc, y */
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TPC], %g2
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TNPC], %g3
  wrpr        %g2, %g0, %tpc
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_Y], %g2
  wrpr        %g3, %g0, %tnpc
  wr          %g2, %g0, %y

  /* if not returning to userspace, skip window changes */
  andcc       %g1, TSTATE_PRIV, %g0
  bnz         %xcc, rtt_kernel
    nop

  wrpr        %g0, (WSTATE_O(0)|WSTATE_N(1)), %wstate

  /* set primary context to secondary */
  wr          %g0, ASI_DMMU, %asi
  ldxa        [SECONDARY_CONTEXT] %asi, %g1
  stxa        %g1, [PRIMARY_CONTEXT] %asi
  membar      #Sync

  /* switch otherwin back to canrestore */
  rdpr        %otherwin, %g1
  wrpr        %g0, %g0, %otherwin
  wrpr        %g0, %g1, %canrestore

  restore

  /* set cleanwin to canrestore and tstate to the correct cwp */
  rdpr        %canrestore, %g1
  wrpr        %g0, %g1, %cleanwin
  rdpr        %cwp, %g1
  wrpr        %g1, %g7, %tstate

  retry

rtt_kernel:
  restore

  /* set tstate to the correct cwp */
  rdpr        %cwp, %g1
  wrpr        %g1, %g7, %tstate

  retry

  .global     rtrap_sc
rtrap_sc:
  /* raise tl, set normal globals and disable interrupts */
  wrpr        %g0, 1, %tl
  wrpr        %g0, PSTATE_PRIV, %pstate

  /* clear the priority interrupt level */
  wrpr        %g0, %g0, %pil

  /* before switching to alternate globals, restore the old ones */
  mov         %l7, %g7
  mov         %l6, %g6
  mov         %l5, %g5
  mov         %l4, %g4
  mov         %l3, %g3
  mov         %l2, %g2
  mov         %l1, %g1

  /* switch to alternate globals and restore tstate with preserved PEF */
  rdpr        %pstate, %l0
  wrpr        %g0, (PSTATE_PRIV|PSTATE_AG), %pstate

  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TSTATE], %g1
  andn        %g1, TSTATE_CWP, %g7
  and         %l0, PSTATE_PEF, %l0
  sllx        %l0, TSTATE_SHIFT_PSTATE, %l0
  sethi       %hi(TSTATE_PEF), %g3
  andn        %g7, %g3, %g1
  or          %g1, %l0, %g1

  /* restore tpc, tnpc, y */
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TPC], %g2
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TNPC], %g3
  wrpr        %g2, %g0, %tpc
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_Y], %g2
  wrpr        %g3, %g0, %tnpc
  wr          %g2, %g0, %y

  wrpr        %g0, (WSTATE_O(0)|WSTATE_N(1)), %wstate

  /* set primary context to secondary */
  wr          %g0, ASI_DMMU, %asi
  ldxa        [SECONDARY_CONTEXT] %asi, %g1
  stxa        %g1, [PRIMARY_CONTEXT] %asi
  membar      #Sync

  /* switch otherwin back to normal */
  rdpr        %otherwin, %g1
  wrpr        %g0, %g0, %otherwin
  wrpr        %g0, %g1, %canrestore

  restore

  /* set cleanwin to canrestore and tstate to the correct cwp */
  rdpr        %canrestore, %g1
  wrpr        %g0, %g1, %cleanwin
  rdpr        %cwp, %g1
  wrpr        %g1, %g7, %tstate

  done


/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
