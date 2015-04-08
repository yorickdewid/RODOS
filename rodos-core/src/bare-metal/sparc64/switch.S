/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief RODOS interfaces, assembly functions for context switching.
 */

#include <asi.h>
#include <pstate.h>
#include <spitfire.h>
#include <syscall.h>
#include <param.h>
#include <trap.h>

  .section    .text
  .align      4
  .register   %g2, #scratch
  .register   %g3, #scratch
  .register   %g6, #scratch
  .register   %g7, #scratch

/* RODOS specific:
 * void __asmSwitchToContext(long* context);
 * void __asmSaveContextAndCallScheduler();
 *
 * The current context pointer __cctx is initially set to the supervisor
 * context __sctx.
 */

  .global __asmSwitchToContext
__asmSwitchToContext:
  /* disable interrupts and enable alternate globals */
  wrpr        %g0,(PSTATE_PRIV|PSTATE_AG), %pstate
  mov         %o0, %g7                    ! save argument in AG g1

  /* Reset window control registers, extract NWINDOWS-1 from %ver */
  rdpr        %ver, %g1                   ! get VERSION register
  and         %g1, VERS_MAXWIN, %g1
  sub         %g1, 1, %g1
  wrpr        %g0, %g0, %cleanwin         ! 0 avoid leakage
  wrpr        %g1, %g0, %cansave          ! NWINDOWS-2
  wrpr        %g0, %g0, %canrestore       ! 0
  wrpr        %g0, %g0, %otherwin         ! 0
  wrpr        %g0, %g0, %cwp

  /* restore ccr and y */
  ldx         [%g7 + (8*33)], %g1
  wr          %g1, %g0, %ccr
  ldx         [%g7 + (8*34)], %g1
  wr          %g1, %g0, %y

  /* normal window spills and fills go to the userspace handlers */
  wrpr        %g0, (WSTATE_O(2)|WSTATE_N(1)), %wstate

  /* switch to context 1 */
  mov         1, %g2
  mov         PRIMARY_CONTEXT, %g1
  stxa        %g2, [%g1] ASI_DMMU
  membar      #Sync
  mov         SECONDARY_CONTEXT, %g1
  stxa        %g2, [%g1] ASI_DMMU
  membar      #Sync

  /* store argument 0 to the __cctx pointer */
  sethi       %hi(__cctx), %g2
  or          %g2, %lo(__cctx), %g2
  stx         %g7, [%g2]
  mov         %g7, %o0                    ! we are about to change globals

  wrpr        %g0,(PSTATE_PRIV), %pstate  ! switch back to normal globals

  /* restore context */
  mov         %o0, %g4

  ldx         [%g4 + (8* 1)], %g1
  ldx         [%g4 + (8* 2)], %g2
  ldx         [%g4 + (8* 3)], %g3
  ! %g4 is cleard afterwards (code model)
  ldx         [%g4 + (8* 5)], %g5
  ldx         [%g4 + (8* 6)], %g6
  ldx         [%g4 + (8* 7)], %g7

  ldx         [%g4 + (8* 8)], %o0
  ldx         [%g4 + (8* 9)], %o1
  ldx         [%g4 + (8*10)], %o2
  ldx         [%g4 + (8*11)], %o3
  ldx         [%g4 + (8*12)], %o4
  ldx         [%g4 + (8*13)], %o5
  ldx         [%g4 + (8*14)], %o6
  ldx         [%g4 + (8*15)], %o7

  ldx         [%g4 + (8*16)], %l0
  ldx         [%g4 + (8*17)], %l1
  ldx         [%g4 + (8*18)], %l2
  ldx         [%g4 + (8*19)], %l3
  ldx         [%g4 + (8*20)], %l4
  ldx         [%g4 + (8*21)], %l5
  ldx         [%g4 + (8*22)], %l6
  ldx         [%g4 + (8*23)], %l7

  ldx         [%g4 + (8*24)], %i0
  ldx         [%g4 + (8*25)], %i1
  ldx         [%g4 + (8*26)], %i2
  ldx         [%g4 + (8*27)], %i3
  ldx         [%g4 + (8*28)], %i4
  ldx         [%g4 + (8*29)], %i5
  ldx         [%g4 + (8*30)], %i6
  ldx         [%g4 + (8*31)], %i7

  clr         %g4

  /* switch to alternate globals and finally load the pc */
  wrpr        %g0,(PSTATE_PRIV|PSTATE_AG), %pstate
  ldx         [%g7 + (8*32)], %g1

  jmpl        %g1, %g0
    wrpr      %g0, (PSTATE_IE|PSTATE_PEF), %pstate



  /* **** Voluntary context switching ****
   * This may be called from userspace.
   */
  .global     __asmSaveContextAndCallScheduler
__asmSaveContextAndCallScheduler:
  /* we have to switch back to supervisor mode for the context switch. In
   * order to accomplish this we have to use a syscall.
   */
  mov         SYS_yield, %g1              ! issues the yield() syscall
  ta          SYS_syscall



  /*
   * SYSCALL handler yield().
   * We arrive here after etrap_sc has run, we want to exit to the
   * schedulerWrapper and continue execution there in supervisor context. In
   * oder to do this we have to do something similar to rtrap_sc but alter
   * the pc and npc values.
   */
  .global     sys_yield
sys_yield:
  /** slightly modified rtrap_sc */
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

  /* switch to alternate globals and restore trap registers */
  wrpr        %g0, (PSTATE_PRIV|PSTATE_AG), %pstate

  /* restore saved trap variables */
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TPC], %g2
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_Y], %g3
  wrpr        %g2, %g0, %tpc
  wr          %g3, %g0, %y

  /* we will exit the syscall trap with a done instruction which does
   *     PC <- TNPC[TL]
   *    nPC <- TNPC[TL]+0x4
   * The syscall handler however intents to return to supervisor mode and
   * continues execution at the schedulerWrapper() function. To archieve this
   * we simply modify TNPC and set it to the address of schedulerWrapper.
   */
  sethi       %hi(schedulerWrapper), %g3
  or          %g3, %lo(schedulerWrapper), %g3
  wrpr        %g3, %g0, %tnpc

  /* enable supervisor mode */
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TSTATE], %g1
  set         (TSTATE_PRIV|TSTATE_PEF), %g2
  andn        %g1, TSTATE_CWP, %g1
  or          %g1, %g2, %g3               ! written to tstate at the end

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

  /* set cleanwin to canrestore */
  rdpr        %canrestore, %g1
  wrpr        %g0, %g1, %cleanwin

  /* make sure callers windows are on callers stack */
  flushw

  /* load ctx pointer and save the current context */
  sethi       %hi(__cctx), %o4
  or          %o4, %lo(__cctx), %o4
  ldx         [%o4], %o0

  stx         %o6, [%o0 + (8*14)]
  stx         %o7, [%o0 + (8*15)]

  stx         %l0, [%o0 + (8*16)]
  stx         %l1, [%o0 + (8*17)]
  stx         %l2, [%o0 + (8*18)]
  stx         %l3, [%o0 + (8*19)]
  stx         %l4, [%o0 + (8*20)]
  stx         %l5, [%o0 + (8*21)]
  stx         %l6, [%o0 + (8*22)]
  stx         %l7, [%o0 + (8*23)]

  stx         %i0, [%o0 + (8*24)]
  stx         %i1, [%o0 + (8*25)]
  stx         %i2, [%o0 + (8*26)]
  stx         %i3, [%o0 + (8*27)]
  stx         %i4, [%o0 + (8*28)]
  stx         %i5, [%o0 + (8*29)]
  stx         %i6, [%o0 + (8*30)]
  stx         %i7, [%o0 + (8*31)]

  add         %o7, 0x8, %g1               ! voluntary, -> new pc %o7+8
  stx         %g1, [%o0 + (8*32)]

  /* copy return value */
  mov         %o0, %i0

  /* set supervisor context */
  sethi       %hi(__sctx), %o3
  or          %o3, %lo(__sctx), %o3
  stx         %o3, [%o4]

  /* Switch to primary context 0 */
  set         boot_stack_top -STACK_BIAS, %g1
  add         %g1, -STACK_FRAME_SZ, %sp
  wrpr        %g0, (WSTATE_O(0)|WSTATE_N(0)), %wstate
  wr          %g0, ASI_DMMU, %asi
  stxa        %g0, [PRIMARY_CONTEXT] %asi
  membar      #Sync

  /* prevent cleanwindow trap */
  wrpr        %g0, (NWINDOWS-2), %cleanwin
  /* set tstate to the correct cwp */
  rdpr        %cwp, %g1
  wrpr        %g1, %g3, %tstate
  done



  /* **** Asynchronous context switching (preemption) ****
   * We arrive here from out tick interrupt handler and want to continue
   * execution at schedulerWrapper in supervisor mode.
   */
  .global     __asmSaveContext
__asmSaveContext:
  /** slightly modified rtrap_irq */
  /* raise tl, set normal globals and disable interrupts */
  wrpr        %g0, 1, %tl
  wrpr        %g0, PSTATE_PRIV, %pstate

  /* clear the priority interrupt level */
  wrpr        %g0, %g0, %pil

  /* when we get here, there are two worthless windows that are left over 
   * from irq_handler and tick_handler
   */
  restore
  restore

  /* before switching to alternate globals, restore the old ones */
  mov         %l7, %g7
  mov         %l6, %g6
  mov         %l5, %g5
  mov         %l4, %g4
  mov         %l3, %g3
  mov         %l2, %g2
  mov         %l1, %g1

  /* switch to alternate globals and restore trap registers */
  wrpr        %g0, (PSTATE_PRIV|PSTATE_AG), %pstate

  /* restore saved trap variables and save old pc */
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_Y], %g3
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TPC], %g2
  wr          %g3, %g0, %y
  sethi       %hi(__cctx), %g3
  or          %g3, %lo(__cctx), %g3
  ldx         [%g3], %g3
  stx         %g2, [%g3 + (8*32)]         ! save old pc

  /* we will exit the interrupt with a retry instruction which does
   *     PC <- TPC[TL]
   *    nPC <- TNPC[TL]
   * The syscall handler however intents to return to supervisor mode and
   * continues execution at the schedulerWrapper() function. To archieve this
   * we simply modify TNPC and set it to the address of schedulerWrapper.
   */
  sethi       %hi(schedulerWrapper), %g3
  or          %g3, %lo(schedulerWrapper), %g3
  wrpr        %g3, %g0, %tpc
  add         %g3, 0x4, %g3
  wrpr        %g3, %g0, %tnpc

  /* enable supervisor mode */
  ldx         [%sp + STACK_BIAS + TRAP_FRAME_SZ - TRAP_TSTATE], %g1
  set         (TSTATE_PRIV|TSTATE_PEF), %g2
  andn        %g1, TSTATE_CWP, %g1
  or          %g1, %g2, %g3               ! written to tstate at the end

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

  /* set cleanwin to canrestore */
  rdpr        %canrestore, %g1
  wrpr        %g0, %g1, %cleanwin

  /* NOTE: We are in the register window that was interrupted, this is
   * precious information that needs saving and cannot be messed up!!
   */

  /* first make sure callers windows are on callers stack */
  flushw

  /* switch to normal globals */
  wrpr        %g0, (PSTATE_PRIV), %pstate

  /* load ctx pointer and save the current context, we can use %g4 for this,
   * since it is the data segment pointer which is always zero.
   */
  sethi       %hi(__cctx), %g4
  or          %g4, %lo(__cctx), %g4
  ldx         [%g4], %g4

  stx         %g1, [%g4 + (8* 1)]
  stx         %g2, [%g4 + (8* 2)]
  stx         %g3, [%g4 + (8* 3)]
  stx         %g0, [%g4 + (8* 4)]
  stx         %g5, [%g4 + (8* 5)]
  stx         %g6, [%g4 + (8* 6)]
  stx         %g7, [%g4 + (8* 7)]

  stx         %o0, [%g4 + (8* 8)]
  stx         %o1, [%g4 + (8* 9)]
  stx         %o2, [%g4 + (8*10)]
  stx         %o3, [%g4 + (8*11)]
  stx         %o4, [%g4 + (8*12)]
  stx         %o5, [%g4 + (8*13)]
  stx         %o6, [%g4 + (8*14)]
  stx         %o7, [%g4 + (8*15)]

  stx         %l0, [%g4 + (8*16)]
  stx         %l1, [%g4 + (8*17)]
  stx         %l2, [%g4 + (8*18)]
  stx         %l3, [%g4 + (8*19)]
  stx         %l4, [%g4 + (8*20)]
  stx         %l5, [%g4 + (8*21)]
  stx         %l6, [%g4 + (8*22)]
  stx         %l7, [%g4 + (8*23)]

  stx         %i0, [%g4 + (8*24)]
  stx         %i1, [%g4 + (8*25)]
  stx         %i2, [%g4 + (8*26)]
  stx         %i3, [%g4 + (8*27)]
  stx         %i4, [%g4 + (8*28)]
  stx         %i5, [%g4 + (8*29)]
  stx         %i6, [%g4 + (8*30)]
  stx         %i7, [%g4 + (8*31)]

  clr         %g4

  /* switch back to alternate globals */
  wrpr        %g0, (PSTATE_PRIV|PSTATE_AG), %pstate

  /* load ctx, copy the return value and save the ccr register */
  sethi       %hi(__cctx), %g1
  or          %g1, %lo(__cctx), %g1
  ldx         [%g1], %o0
  mov         %o0, %i0
  sethi       %hi(TSTATE_CCR), %g5
  and         %g3, %g5, %g5
  srlx        %g5, TSTATE_SHIFT_CCR, %g5
  stx         %g5, [%o0 + (8*33)]
  rd          %y, %g5
  stx         %g5, [%o0 + (8*34)]

  /* and set supervisor context */
  sethi       %hi(__sctx), %o3
  or          %o3, %lo(__sctx), %o3
  stx         %o3, [%g1]

  /* Switch to primary context 0 */
  set         boot_stack_top -STACK_BIAS, %g1
  add         %g1, -STACK_FRAME_SZ, %sp
  wrpr        %g0, (WSTATE_O(0)|WSTATE_N(0)), %wstate
  wr          %g0, ASI_DMMU, %asi
  stxa        %g0, [PRIMARY_CONTEXT] %asi
  membar      #Sync

  /* prevent cleanwindow trap */
  wrpr        %g0, (NWINDOWS-2), %cleanwin
  /* set tstate to the correct cwp */
  rdpr        %cwp, %g1
  wrpr        %g1, %g3, %tstate
  retry


/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
