/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief The kernel trap table.
 * The preprocessor directives are inspired by the way linux does it.
 */

#include <trap.h>
#include <asi.h>
#include <param.h>

  .register   %g2, #scratch
  .register   %g3, #scratch
  .register   %g6, #scratch
  .register   %g7, #scratch

/* The trap table is located in memory at the address that is stored in
 * the TBA (trap table base address) register during boot (see head.S).
 * The displacement is defined by TL and TT (trap level and trap type).
 *
 * We implement two trap tables with 512 trap handler slots each:
 *    tl0 - for traps at trap level = 0
 *    tl1 - for traps at trap level > 0
 *
 * The 512 trap handlers
 *    64 hardware traps
 *    16 irqs
 *    16 reserved
 *    32 architecture dependent
 *    128 32 window fill/spill each with 4 slots
 *    128 software traps
 *    128 reserved
 *
 * Trap-table-entry-points on SPARCv9 are 8 instructions in size, spill/fill
 * traps even 32 instructions; Note, on v7/v8 each entry point was only 4
 * instructions in size. We'll need to fill the remainder with nop's, .skip's
 * or .align <N> because we must not mess up the trap table size. One half of
 * each trap table is used for hardware traps, another quarter is reserved for
 * software initiated traps and the last quarter is reserved for future
 * implementations.
 *
 * Provided trap handlers to dispatch traps into C code:
 *    BTRAPTL0(lvl)           - bad trap tl=0
 *    BTRAPTL1(lvl)           - bad trap tl>0
 *
 *    TRAP_TL0(routine)       - normal trap handling
 *    TRAP_TL1(routine)       - higher level trap handling
 *
 *    TRAP_EXC(routine)       - exception, no save
 *    TRAP_IRQ(routine, lvl)  - interrupt request
 */

/** Hardware reset. */
#define BOOT_START                                  \
  b sparc64_boot_start                              ;\
    nop                                             ;\
  nop; nop; nop; nop; nop; nop;

/**
 * Trap handler for the 'cleaned' instruction.
 * There are 32 instructions to be filled since we have 4 trap slots.
 */
#define CLEAN_WINDOW                                \
  rdpr  %cleanwin, %l0                              ;\
  add   %l0, 1, %l0                                 ;\
  wrpr  %l0, 0, %cleanwin                           ;\
  clr   %o0                                         ;\
  clr   %o1                                         ;\
  clr   %o2                                         ;\
  clr   %o3                                         ;\
  clr   %o4                                         ;\
  clr   %o5                                         ;\
  clr   %o6                                         ;\
  clr   %o7                                         ;\
  clr   %l0                                         ;\
  clr   %l1                                         ;\
  clr   %l2                                         ;\
  clr   %l3                                         ;\
  clr   %l4                                         ;\
  clr   %l5                                         ;\
  clr   %l6                                         ;\
  clr   %l7                                         ;\
  retry                                             ;\
  nop; nop; nop; nop; nop; nop;                     \
  nop; nop; nop; nop; nop; nop;

/**
 * Bad trap handler TL = 0.
 */
#define BTRAPTL0(arg)                               \
  ba,pt   %xcc, etrap                               ;\
    rd    %pc, %g7                                  ;\
  add     %sp, (STACK_BIAS + TREGS_OFFSET), %o1     ;\
  call    bad_trap                                  ;\
    mov   arg, %o0                                  ;\
  ba,pt   %xcc, rtrap                               ;\
    nop                                             ;\
  nop;

/**
 * Bad trap handler TL > 0.
 * Currently just the same as BTRAPTL0.
 */
#define BTRAPTL1(arg)                               \
  ba,pt   %xcc, etrap_tl1                           ;\
    rd    %pc, %g7                                  ;\
  add     %sp, (STACK_BIAS + TREGS_OFFSET), %o1     ;\
  call    bad_trap                                  ;\
    mov   arg, %o0                                  ;\
  ba,pt   %xcc, rtrap_tl1                           ;\
    nop                                             ;\
  nop;

/**
 * Trap handler for TL = 0.
 */
#define TRAP_TL0(routine)                           \
  ba,pt   %xcc, etrap                               ;\
    rd    %pc, %g7                                  ;\
  call    routine                                   ;\
    add   %sp, (STACK_BIAS + TREGS_OFFSET), %o0     ;\
  ba,pt   %xcc, rtrap                               ;\
    nop                                             ;\
  nop; nop;

/**
 * Trap handler for TL > 0.
 */
#define TRAP_TL1(routine)                           \
  ba,pt   %xcc, etrap_tl1                           ;\
    rd    %pc, %g7                                  ;\
  call    routine                                   ;\
    add   %sp, (STACK_BIAS + TREGS_OFFSET), %o0     ;\
  ba,pt   %xcc, rtrap_tl1                           ;\
    nop                                             ;\
  nop; nop;

/**
 * Exception handler.
 */
#define TRAP_EXC(routine)                           \
  ba,pt   %xcc, routine                             ;\
    nop                                             ;\
  nop; nop; nop; nop; nop; nop;

/**
 * Interrupt request trap handler.
 */
#define TRAP_IRQ(routine, lvl)                      \
  rdpr    %pil, %g2                                 ;\
  wrpr    %g0, lvl, %pil                            ;\
  ba,pt   %xcc, etrap_irq                           ;\
    rd    %pc, %g7                                  ;\
  mov     lvl, %o0                                  ;\
  call    routine                                   ;\
    add    %sp, (STACK_BIAS + TREGS_OFFSET), %o1    ;\
  ba,a,pt %xcc, rtrap_irq;

/**
 * System call handler.
 */
#define SYS_TRAP(handler)                           \
  ba,pt   %xcc, etrap_sc                            ;\
    rd    %pc, %g7                                  ;\
  call    handler                                   ;\
    nop;                                            ;\
  ba,pt   %xcc, rtrap_sc                            ;\
    nop                                             ;\
  nop; nop;

#define TRAP_FLUSH_WINDOWS                          \
  ba,pt   %xcc, etrap                               ;\
    rd    %pc, %g7                                  ;\
  flushw                                            ;\
  ba,pt   %xcc, rtrap                               ;\
    nop                                             ;\
  nop; nop; nop;

/**
 * Spill a register window (64-bit mode).
 * CWP is set, now save the window to the stack.
 */
#define TRAP_NORMAL_SPILL_0                         \
  stx   %l0, [%sp + STACK_BIAS + (8* 0)]            ;\
  stx   %l1, [%sp + STACK_BIAS + (8* 1)]            ;\
  stx   %l2, [%sp + STACK_BIAS + (8* 2)]            ;\
  stx   %l3, [%sp + STACK_BIAS + (8* 3)]            ;\
  stx   %l4, [%sp + STACK_BIAS + (8* 4)]            ;\
  stx   %l5, [%sp + STACK_BIAS + (8* 5)]            ;\
  stx   %l6, [%sp + STACK_BIAS + (8* 6)]            ;\
  stx   %l7, [%sp + STACK_BIAS + (8* 7)]            ;\
  stx   %i0, [%sp + STACK_BIAS + (8* 8)]            ;\
  stx   %i1, [%sp + STACK_BIAS + (8* 9)]            ;\
  stx   %i2, [%sp + STACK_BIAS + (8*10)]            ;\
  stx   %i3, [%sp + STACK_BIAS + (8*11)]            ;\
  stx   %i4, [%sp + STACK_BIAS + (8*12)]            ;\
  stx   %i5, [%sp + STACK_BIAS + (8*13)]            ;\
  stx   %i6, [%sp + STACK_BIAS + (8*14)]            ;\
  stx   %i7, [%sp + STACK_BIAS + (8*15)]            ;\
  saved                                             ;\
  retry                                             ;\
  nop; nop; nop; nop; nop; nop; nop                 ;\
  nop; nop; nop; nop; nop; nop; nop;

/** Spill trap user PRIMARY. */
#define TRAP_NORMAL_SPILL_USERP                     \
  wr    %g0, ASI_AIUP, %asi                         ;\
  stxa  %l0, [%sp + STACK_BIAS + (8* 0)] %asi       ;\
  stxa  %l1, [%sp + STACK_BIAS + (8* 1)] %asi       ;\
  stxa  %l2, [%sp + STACK_BIAS + (8* 2)] %asi       ;\
  stxa  %l3, [%sp + STACK_BIAS + (8* 3)] %asi       ;\
  stxa  %l4, [%sp + STACK_BIAS + (8* 4)] %asi       ;\
  stxa  %l5, [%sp + STACK_BIAS + (8* 5)] %asi       ;\
  stxa  %l6, [%sp + STACK_BIAS + (8* 6)] %asi       ;\
  stxa  %l7, [%sp + STACK_BIAS + (8* 7)] %asi       ;\
  stxa  %i0, [%sp + STACK_BIAS + (8* 8)] %asi       ;\
  stxa  %i1, [%sp + STACK_BIAS + (8* 9)] %asi       ;\
  stxa  %i2, [%sp + STACK_BIAS + (8*10)] %asi       ;\
  stxa  %i3, [%sp + STACK_BIAS + (8*11)] %asi       ;\
  stxa  %i4, [%sp + STACK_BIAS + (8*12)] %asi       ;\
  stxa  %i5, [%sp + STACK_BIAS + (8*13)] %asi       ;\
  stxa  %i6, [%sp + STACK_BIAS + (8*14)] %asi       ;\
  stxa  %i7, [%sp + STACK_BIAS + (8*15)] %asi       ;\
  saved                                             ;\
  retry                                             ;\
  nop; nop; nop; nop; nop; nop; nop                 ;\
  nop; nop; nop; nop; nop; nop;

/** Spill trap user SECONDARY. */
#define TRAP_NORMAL_SPILL_USERS                     \
  wr    %g0, ASI_AIUS, %asi                         ;\
  stxa  %l0, [%sp + STACK_BIAS + (8* 0)] %asi       ;\
  stxa  %l1, [%sp + STACK_BIAS + (8* 1)] %asi       ;\
  stxa  %l2, [%sp + STACK_BIAS + (8* 2)] %asi       ;\
  stxa  %l3, [%sp + STACK_BIAS + (8* 3)] %asi       ;\
  stxa  %l4, [%sp + STACK_BIAS + (8* 4)] %asi       ;\
  stxa  %l5, [%sp + STACK_BIAS + (8* 5)] %asi       ;\
  stxa  %l6, [%sp + STACK_BIAS + (8* 6)] %asi       ;\
  stxa  %l7, [%sp + STACK_BIAS + (8* 7)] %asi       ;\
  stxa  %i0, [%sp + STACK_BIAS + (8* 8)] %asi       ;\
  stxa  %i1, [%sp + STACK_BIAS + (8* 9)] %asi       ;\
  stxa  %i2, [%sp + STACK_BIAS + (8*10)] %asi       ;\
  stxa  %i3, [%sp + STACK_BIAS + (8*11)] %asi       ;\
  stxa  %i4, [%sp + STACK_BIAS + (8*12)] %asi       ;\
  stxa  %i5, [%sp + STACK_BIAS + (8*13)] %asi       ;\
  stxa  %i6, [%sp + STACK_BIAS + (8*14)] %asi       ;\
  stxa  %i7, [%sp + STACK_BIAS + (8*15)] %asi       ;\
  saved                                             ;\
  retry                                             ;\
  nop; nop; nop; nop; nop; nop; nop                 ;\
  nop; nop; nop; nop; nop; nop;


#define TRAP_NORMAL_SPILL_1 TRAP_NORMAL_SPILL_USERP
#define TRAP_NORMAL_SPILL_2 TRAP_NORMAL_SPILL_USERS
#define TRAP_NORMAL_SPILL_3 TRAP_NORMAL_SPILL_0
#define TRAP_NORMAL_SPILL_4 TRAP_NORMAL_SPILL_0
#define TRAP_NORMAL_SPILL_5 TRAP_NORMAL_SPILL_0
#define TRAP_NORMAL_SPILL_6 TRAP_NORMAL_SPILL_0
#define TRAP_NORMAL_SPILL_7 TRAP_NORMAL_SPILL_0

#define TRAP_OTHER_SPILL_0 TRAP_NORMAL_SPILL_0
#define TRAP_OTHER_SPILL_1 TRAP_NORMAL_SPILL_1
#define TRAP_OTHER_SPILL_2 TRAP_NORMAL_SPILL_2
#define TRAP_OTHER_SPILL_3 TRAP_NORMAL_SPILL_3
#define TRAP_OTHER_SPILL_4 TRAP_NORMAL_SPILL_4
#define TRAP_OTHER_SPILL_5 TRAP_NORMAL_SPILL_5
#define TRAP_OTHER_SPILL_6 TRAP_NORMAL_SPILL_6
#define TRAP_OTHER_SPILL_7 TRAP_NORMAL_SPILL_7

/**
 * Fill a register window (64-bit mode).
 * CWP is set, now restore the previously spilled window.
 */
#define TRAP_NORMAL_FILL_0                          \
  ldx   [%sp + STACK_BIAS + (8* 0)], %l0            ;\
  ldx   [%sp + STACK_BIAS + (8* 1)], %l1            ;\
  ldx   [%sp + STACK_BIAS + (8* 2)], %l2            ;\
  ldx   [%sp + STACK_BIAS + (8* 3)], %l3            ;\
  ldx   [%sp + STACK_BIAS + (8* 4)], %l4            ;\
  ldx   [%sp + STACK_BIAS + (8* 5)], %l5            ;\
  ldx   [%sp + STACK_BIAS + (8* 6)], %l6            ;\
  ldx   [%sp + STACK_BIAS + (8* 7)], %l7            ;\
  ldx   [%sp + STACK_BIAS + (8* 8)], %i0            ;\
  ldx   [%sp + STACK_BIAS + (8* 9)], %i1            ;\
  ldx   [%sp + STACK_BIAS + (8*10)], %i2            ;\
  ldx   [%sp + STACK_BIAS + (8*11)], %i3            ;\
  ldx   [%sp + STACK_BIAS + (8*12)], %i4            ;\
  ldx   [%sp + STACK_BIAS + (8*13)], %i5            ;\
  ldx   [%sp + STACK_BIAS + (8*14)], %i6            ;\
  ldx   [%sp + STACK_BIAS + (8*15)], %i7            ;\
  restored                                          ;\
  retry                                             ;\
  nop; nop; nop; nop; nop; nop; nop                 ;\
  nop; nop; nop; nop; nop; nop; nop;

/** Fill trap user PRIMARY. */
#define TRAP_NORMAL_FILL_USERP                      \
  wr    %g0, ASI_AIUP, %asi                         ;\
  ldxa  [%sp + STACK_BIAS + (8* 0)] %asi, %l0       ;\
  ldxa  [%sp + STACK_BIAS + (8* 1)] %asi, %l1       ;\
  ldxa  [%sp + STACK_BIAS + (8* 2)] %asi, %l2       ;\
  ldxa  [%sp + STACK_BIAS + (8* 3)] %asi, %l3       ;\
  ldxa  [%sp + STACK_BIAS + (8* 4)] %asi, %l4       ;\
  ldxa  [%sp + STACK_BIAS + (8* 5)] %asi, %l5       ;\
  ldxa  [%sp + STACK_BIAS + (8* 6)] %asi, %l6       ;\
  ldxa  [%sp + STACK_BIAS + (8* 7)] %asi, %l7       ;\
  ldxa  [%sp + STACK_BIAS + (8* 8)] %asi, %i0       ;\
  ldxa  [%sp + STACK_BIAS + (8* 9)] %asi, %i1       ;\
  ldxa  [%sp + STACK_BIAS + (8*10)] %asi, %i2       ;\
  ldxa  [%sp + STACK_BIAS + (8*11)] %asi, %i3       ;\
  ldxa  [%sp + STACK_BIAS + (8*12)] %asi, %i4       ;\
  ldxa  [%sp + STACK_BIAS + (8*13)] %asi, %i5       ;\
  ldxa  [%sp + STACK_BIAS + (8*14)] %asi, %i6       ;\
  ldxa  [%sp + STACK_BIAS + (8*15)] %asi, %i7       ;\
  restored                                          ;\
  retry                                             ;\
  nop; nop; nop; nop; nop; nop; nop                 ;\
  nop; nop; nop; nop; nop; nop;

/** Fill trap user SECONDARY. */
#define TRAP_NORMAL_FILL_USERS                      \
  wr    %g0, ASI_AIUS, %asi                         ;\
  ldxa  [%sp + STACK_BIAS + (8* 0)] %asi, %l0       ;\
  ldxa  [%sp + STACK_BIAS + (8* 1)] %asi, %l1       ;\
  ldxa  [%sp + STACK_BIAS + (8* 2)] %asi, %l2       ;\
  ldxa  [%sp + STACK_BIAS + (8* 3)] %asi, %l3       ;\
  ldxa  [%sp + STACK_BIAS + (8* 4)] %asi, %l4       ;\
  ldxa  [%sp + STACK_BIAS + (8* 5)] %asi, %l5       ;\
  ldxa  [%sp + STACK_BIAS + (8* 6)] %asi, %l6       ;\
  ldxa  [%sp + STACK_BIAS + (8* 7)] %asi, %l7       ;\
  ldxa  [%sp + STACK_BIAS + (8* 8)] %asi, %i0       ;\
  ldxa  [%sp + STACK_BIAS + (8* 9)] %asi, %i1       ;\
  ldxa  [%sp + STACK_BIAS + (8*10)] %asi, %i2       ;\
  ldxa  [%sp + STACK_BIAS + (8*11)] %asi, %i3       ;\
  ldxa  [%sp + STACK_BIAS + (8*12)] %asi, %i4       ;\
  ldxa  [%sp + STACK_BIAS + (8*13)] %asi, %i5       ;\
  ldxa  [%sp + STACK_BIAS + (8*14)] %asi, %i6       ;\
  ldxa  [%sp + STACK_BIAS + (8*15)] %asi, %i7       ;\
  restored                                          ;\
  retry                                             ;\
  nop; nop; nop; nop; nop; nop; nop                 ;\
  nop; nop; nop; nop; nop; nop;


#define TRAP_NORMAL_FILL_1 TRAP_NORMAL_FILL_USERP
#define TRAP_NORMAL_FILL_2 TRAP_NORMAL_FILL_USERS
#define TRAP_NORMAL_FILL_3 TRAP_NORMAL_FILL_0
#define TRAP_NORMAL_FILL_4 TRAP_NORMAL_FILL_0
#define TRAP_NORMAL_FILL_5 TRAP_NORMAL_FILL_0
#define TRAP_NORMAL_FILL_6 TRAP_NORMAL_FILL_0
#define TRAP_NORMAL_FILL_7 TRAP_NORMAL_FILL_0

#define TRAP_OTHER_FILL_0 TRAP_NORMAL_FILL_0
#define TRAP_OTHER_FILL_1 TRAP_NORMAL_FILL_1
#define TRAP_OTHER_FILL_2 TRAP_NORMAL_FILL_2
#define TRAP_OTHER_FILL_3 TRAP_NORMAL_FILL_3
#define TRAP_OTHER_FILL_4 TRAP_NORMAL_FILL_4
#define TRAP_OTHER_FILL_5 TRAP_NORMAL_FILL_5
#define TRAP_OTHER_FILL_6 TRAP_NORMAL_FILL_6
#define TRAP_OTHER_FILL_7 TRAP_NORMAL_FILL_7

/* The v9 specs require the trap table to be size aligned.
 *    2 tables * 512 entries * 8 instructions * 4 bytes instruction size
 *    = 32768 bytes (0x8000)
 */
  .section    .text
  .align      32768
  .global     sparc64_ttable_tl0, sparc64_ttable_tl1
/*
 * Trap table for trap level TL = 0
 */
sparc64_ttable_tl0:
                    /* 0x000 reserved, hw reset */
                    BOOT_START
                    BTRAPTL0(0x001) /* power on reset */
                    BTRAPTL0(0x002) /* watchdog reset */
                    BTRAPTL0(0x003) /* externally initiated reset */
                    BTRAPTL0(0x004) /* software initiated reset */
                    BTRAPTL0(0x005) /* RED state exception */
                    BTRAPTL0(0x006) /* reserved */
                    BTRAPTL0(0x007) /* reserved */
                    /* 0x008 instruction access exception */
tl0_iax:            TRAP_EXC(trap_instruction_access_exception)
tl0_resv009:        BTRAPTL0(0x009) /* instruction access MMU miss */
                    /* 0x00a instruction access error */
tl0_iae:            TRAP_TL0(trap_instruction_access_error)
tl0_resv00b:        BTRAPTL0(0x00b) /* reserved */
                    BTRAPTL0(0x00c) /* reserved */
                    BTRAPTL0(0x00d) /* reserved */
                    BTRAPTL0(0x00e) /* reserved */
                    BTRAPTL0(0x00f) /* reserved */
                    /* illegal instruction */
tl0_illinst:        TRAP_TL0(trap_illegal_instruction)
                    /* privileged opcode (instruction) */
tl0_privop:         TRAP_TL0(trap_privileged_opcode)
tl0_unimpldd:       BTRAPTL0(0x012) /* unimplemented LDD */
tl0_unimpstd:       BTRAPTL0(0x013) /* unimplemented STD */
tl0_resv014:        BTRAPTL0(0x014) /* reserved */
                    BTRAPTL0(0x015) /* reserved */
                    BTRAPTL0(0x016) /* reserved */
                    BTRAPTL0(0x017) /* reserved */
                    BTRAPTL0(0x018) /* reserved */
                    BTRAPTL0(0x019) /* reserved */
                    BTRAPTL0(0x01a) /* reserved */
                    BTRAPTL0(0x01b) /* reserved */
                    BTRAPTL0(0x01c) /* reserved */
                    BTRAPTL0(0x01d) /* reserved */
                    BTRAPTL0(0x01e) /* reserved */
                    BTRAPTL0(0x01f) /* reserved */
                    /* floating point disabled */
tl0_fpdis:          TRAP_TL0(trap_fp_disabled)
                    BTRAPTL0(0x021) /* ieee 754 floating point exception */
                    BTRAPTL0(0x022) /* other floating point exception */

                    BTRAPTL0(0x023) /* tag overflow */
                    /* 0x024 clean window (4 slots = 32 instructions) */
tl0_cleanwin:       CLEAN_WINDOW
                    BTRAPTL0(0x028) /* division by zero */
tl0_iperr:          BTRAPTL0(0x029) /* internal processor error */
tl0_resv02a:        BTRAPTL0(0x02a) /* reserved */
                    BTRAPTL0(0x02b) /* reserved */
                    BTRAPTL0(0x02c) /* reserved */
                    BTRAPTL0(0x02d) /* reserved */
                    BTRAPTL0(0x02e) /* reserved */
                    BTRAPTL0(0x02f) /* reserved */
                    /* data access exception */
tl0_dax:            TRAP_EXC(trap_data_accesss_exception)
tl0_damm:           BTRAPTL0(0x031) /* data access MMU miss */
                    BTRAPTL0(0x032) /* data access error */
tl0_dap:            BTRAPTL0(0x033) /* data access protection */
                    /* memory address not aligned */
tl0_mna:            TRAP_EXC(trap_mem_address_not_aligned)
                    BTRAPTL0(0x035) /* lddf memory address not aligned */
                    BTRAPTL0(0x036) /* stdf memory address not aligned */
                    BTRAPTL0(0x037) /* privileged action (eg: asi access) */
tl0_ldqfmna:        BTRAPTL0(0x038) /* ldqf memory address not aligned */
tl0_stqfmna:        BTRAPTL0(0x039) /* stqf memory address not aligned */
tl0_resv03a:        BTRAPTL0(0x03a) /* reserved */
                    BTRAPTL0(0x03b) /* reserved */
                    BTRAPTL0(0x03c) /* reserved */
                    BTRAPTL0(0x03d) /* reserved */
                    BTRAPTL0(0x03e) /* reserved */
                    BTRAPTL0(0x03f) /* reserved */
tl0_aderr:          BTRAPTL0(0x040) /* asynchronous data error (ECC) */
                    /* Interrupt Level  1 implementation dependent */
tl0_irq01:          TRAP_IRQ(irq_handler, 1)
                    /* Interrupt Level  2 sbus/VME lvl 1 */
tl0_irq02:          TRAP_IRQ(irq_handler, 2)
                    /* Interrupt Level  3 sbus/VME lvl 2 */
tl0_irq03:          TRAP_IRQ(irq_handler, 3)
                    /* Interrupt Level  4 onboard scsi */
tl0_irq04:          TRAP_IRQ(irq_handler, 4)
                    /* Interrupt Level  5 sbus/VME lvl 3 */
tl0_irq05:          TRAP_IRQ(irq_handler, 5)
                    /* Interrupt Level  6 onboard ethernet */
tl0_irq06:          TRAP_IRQ(irq_handler, 6)
                    /* Interrupt Level  7 sbus/VME lvl 4 */
tl0_irq07:          TRAP_IRQ(irq_handler, 7)
                    /* Interrupt Level  8 onboard video */
tl0_irq08:          TRAP_IRQ(irq_handler, 8)
                    /* Interrupt Level  9 sbus/VME lvl 5,module */
tl0_irq09:          TRAP_IRQ(irq_handler, 9)
                    /* Interrupt Level 10 TOD clock */
tl0_irq10:          TRAP_IRQ(irq_handler,10)
                    /* Interrupt Level 11 sbus/VME lvl 6,floppy */
tl0_irq11:          TRAP_IRQ(irq_handler,11)
                    /* Interrupt Level 12 serial i/o */
tl0_irq12:          TRAP_IRQ(irq_handler,12)
                    /* Interrupt Level 13 sbus/VME lvl 7, audio, X-call */
tl0_irq13:          TRAP_IRQ(irq_handler,13)
                    /* Interrupt Level 14 hi-res system timer "tick irq" */
tl0_irq14:          TRAP_IRQ(irq_handler,14)
                    /* Interrupt Level 15 asynchronous memory fault */
tl0_irq15:          TRAP_IRQ(irq_handler,15)
tl0_resv050:        BTRAPTL0(0x050) /* reserved */
                    BTRAPTL0(0x051) /* reserved */
                    BTRAPTL0(0x052) /* reserved */
                    BTRAPTL0(0x053) /* reserved */
                    BTRAPTL0(0x054) /* reserved */
                    BTRAPTL0(0x055) /* reserved */
                    BTRAPTL0(0x056) /* reserved */
                    BTRAPTL0(0x057) /* reserved */
                    BTRAPTL0(0x058) /* reserved */
                    BTRAPTL0(0x059) /* reserved */
                    BTRAPTL0(0x05a) /* reserved */
                    BTRAPTL0(0x05b) /* reserved */
                    BTRAPTL0(0x05c) /* reserved */
                    BTRAPTL0(0x05d) /* reserved */
                    BTRAPTL0(0x05e) /* reserved */
                    BTRAPTL0(0x05f) /* reserved */
/* Implementation dependent definitions and priorities. */
                    /* 0x060 interrupt vector */
tl0_ivec:           TRAP_TL0(vec_interrupt)
                    BTRAPTL0(0x061) /* physical address watchpoint */
                    BTRAPTL0(0x062) /* virtual address watchpoint */
                    BTRAPTL0(0x063) /* corrected ECC error */
                    /* 0x64 instruction access MMU miss (4-slots) */
tl0_iamiss:
#include "mm/itlb_miss.S"
                    /* 0x68 data access MMU miss (4-slots) */
tl0_damiss:
#include "mm/dtlb_miss.S"
                    /* 0x6c data access protection (4-slots) */
tl0_daprot:
#include "mm/dtlb_prot.S"
tl0_resv070:        BTRAPTL0(0x070) /* implementation dependent */
                    BTRAPTL0(0x071) /* implementation dependent */
                    BTRAPTL0(0x072) /* implementation dependent */
                    BTRAPTL0(0x073) /* implementation dependent */
                    BTRAPTL0(0x074) /* implementation dependent */
                    BTRAPTL0(0x075) /* implementation dependent */
                    BTRAPTL0(0x076) /* implementation dependent */
                    BTRAPTL0(0x077) /* implementation dependent */
                    BTRAPTL0(0x078) /* implementation dependent */
                    BTRAPTL0(0x079) /* implementation dependent */
                    BTRAPTL0(0x07a) /* implementation dependent */
                    BTRAPTL0(0x07b) /* implementation dependent */
                    BTRAPTL0(0x07c) /* implementation dependent */
                    BTRAPTL0(0x07d) /* implementation dependent */
                    BTRAPTL0(0x07e) /* implementation dependent */
                    BTRAPTL0(0x07f) /* implementation dependent */
/* spill/fill traps, all of these are 32 instructions in size. */
                    /* otherwin  = 0, wstate.normal = 0..7 spill, normal */
                    /* 0x080 ... 0x09c */
tl0_s0n:            TRAP_NORMAL_SPILL_0
tl0_s1n:            TRAP_NORMAL_SPILL_1
tl0_s2n:            TRAP_NORMAL_SPILL_2
tl0_s3n:            TRAP_NORMAL_SPILL_3
tl0_s4n:            TRAP_NORMAL_SPILL_4
tl0_s5n:            TRAP_NORMAL_SPILL_5
tl0_s6n:            TRAP_NORMAL_SPILL_6
tl0_s7n:            TRAP_NORMAL_SPILL_7
                    /* otherwin != 0, wstate.normal = 0..7 spill, other */
                    /* 0x0a0 ... 0x0bc */
tl0_s0o:            TRAP_OTHER_SPILL_0
tl0_s1o:            TRAP_OTHER_SPILL_1
tl0_s2o:            TRAP_OTHER_SPILL_2
tl0_s3o:            TRAP_OTHER_SPILL_3
tl0_s4o:            TRAP_OTHER_SPILL_4
tl0_s5o:            TRAP_OTHER_SPILL_5
tl0_s6o:            TRAP_OTHER_SPILL_6
tl0_s7o:            TRAP_OTHER_SPILL_7
                    /* otherwin  = 0, wstate.normal = 0..7  fill, normal */
                    /* 0x0c0 ... 0x0dc */
tl0_f0n:            TRAP_NORMAL_FILL_0
tl0_f1n:            TRAP_NORMAL_FILL_1
tl0_f2n:            TRAP_NORMAL_FILL_2
tl0_f3n:            TRAP_NORMAL_FILL_3
tl0_f4n:            TRAP_NORMAL_FILL_4
tl0_f5n:            TRAP_NORMAL_FILL_5
tl0_f6n:            TRAP_NORMAL_FILL_6
tl0_f7n:            TRAP_NORMAL_FILL_7
                    /* otherwin != 0, wstate.normal = 0..7  fill, other */
                    /* 0x0e0 ... 0x0fc */
tl0_f0o:            TRAP_OTHER_FILL_0
tl0_f1o:            TRAP_OTHER_FILL_1
tl0_f2o:            TRAP_OTHER_FILL_2
tl0_f3o:            TRAP_OTHER_FILL_3
tl0_f4o:            TRAP_OTHER_FILL_4
tl0_f5o:            TRAP_OTHER_FILL_5
tl0_f6o:            TRAP_OTHER_FILL_6
tl0_f7o:            TRAP_OTHER_FILL_7
/* sofware traps */
                    /* 0x100 system call */
tl0_syscall:        SYS_TRAP(system_call)
                    /* 0x101 user breakpoint */
tl0_ubrkp:          BTRAPTL0(0x101)
                    /* 0x102 userland divide by zero */
tl0_udivzero:       BTRAPTL0(0x102)
                    /* 0x103 flush windows */
tl0_flushw:         TRAP_FLUSH_WINDOWS
tl0_resv104:        BTRAPTL0(0x104) /* not used */
                    BTRAPTL0(0x105) /* not used */
                    BTRAPTL0(0x106) /* not used */
                    BTRAPTL0(0x107) /* not used */
                    BTRAPTL0(0x108) /* not used */
                    BTRAPTL0(0x109) /* not used */
                    BTRAPTL0(0x10a) /* not used */
                    BTRAPTL0(0x10b) /* not used */
                    BTRAPTL0(0x10c) /* not used */
                    BTRAPTL0(0x10d) /* not used */
                    BTRAPTL0(0x10e) /* not used */
                    BTRAPTL0(0x10f) /* not used */
                    BTRAPTL0(0x110) /* not used */
                    BTRAPTL0(0x111) /* not used */
                    BTRAPTL0(0x112) /* not used */
                    BTRAPTL0(0x113) /* not used */
                    BTRAPTL0(0x114) /* not used */
                    BTRAPTL0(0x115) /* not used */
                    BTRAPTL0(0x116) /* not used */
                    BTRAPTL0(0x117) /* not used */
                    BTRAPTL0(0x118) /* not used */
                    BTRAPTL0(0x119) /* not used */
                    BTRAPTL0(0x11a) /* not used */
                    BTRAPTL0(0x11b) /* not used */
                    BTRAPTL0(0x11c) /* not used */
                    BTRAPTL0(0x11d) /* not used */
                    BTRAPTL0(0x11e) /* not used */
                    BTRAPTL0(0x11f) /* not used */
                    BTRAPTL0(0x120) /* not used */
                    BTRAPTL0(0x121) /* not used */
                    BTRAPTL0(0x122) /* not used */
                    BTRAPTL0(0x123) /* not used */
                    BTRAPTL0(0x124) /* not used */
                    BTRAPTL0(0x125) /* not used */
                    BTRAPTL0(0x126) /* not used */
                    BTRAPTL0(0x127) /* not used */
                    BTRAPTL0(0x128) /* not used */
                    BTRAPTL0(0x129) /* not used */
                    BTRAPTL0(0x12a) /* not used */
                    BTRAPTL0(0x12b) /* not used */
                    BTRAPTL0(0x12c) /* not used */
                    BTRAPTL0(0x12d) /* not used */
                    BTRAPTL0(0x12e) /* not used */
                    BTRAPTL0(0x12f) /* not used */
                    BTRAPTL0(0x130) /* not used */
                    BTRAPTL0(0x131) /* not used */
                    BTRAPTL0(0x132) /* not used */
                    BTRAPTL0(0x133) /* not used */
                    BTRAPTL0(0x134) /* not used */
                    BTRAPTL0(0x135) /* not used */
                    BTRAPTL0(0x136) /* not used */
                    BTRAPTL0(0x137) /* not used */
                    BTRAPTL0(0x138) /* not used */
                    BTRAPTL0(0x139) /* not used */
                    BTRAPTL0(0x13a) /* not used */
                    BTRAPTL0(0x13b) /* not used */
                    BTRAPTL0(0x13c) /* not used */
                    BTRAPTL0(0x13d) /* not used */
                    BTRAPTL0(0x13e) /* not used */
                    BTRAPTL0(0x13f) /* not used */
                    BTRAPTL0(0x140) /* not used */
                    BTRAPTL0(0x141) /* not used */
                    BTRAPTL0(0x142) /* not used */
                    BTRAPTL0(0x143) /* not used */
                    BTRAPTL0(0x144) /* not used */
                    BTRAPTL0(0x145) /* not used */
                    BTRAPTL0(0x146) /* not used */
                    BTRAPTL0(0x147) /* not used */
                    BTRAPTL0(0x148) /* not used */
                    BTRAPTL0(0x149) /* not used */
                    BTRAPTL0(0x14a) /* not used */
                    BTRAPTL0(0x14b) /* not used */
                    BTRAPTL0(0x14c) /* not used */
                    BTRAPTL0(0x14d) /* not used */
                    BTRAPTL0(0x14e) /* not used */
                    BTRAPTL0(0x14f) /* not used */
                    BTRAPTL0(0x150) /* not used */
                    BTRAPTL0(0x151) /* not used */
                    BTRAPTL0(0x152) /* not used */
                    BTRAPTL0(0x153) /* not used */
                    BTRAPTL0(0x154) /* not used */
                    BTRAPTL0(0x155) /* not used */
                    BTRAPTL0(0x156) /* not used */
                    BTRAPTL0(0x157) /* not used */
                    BTRAPTL0(0x158) /* not used */
                    BTRAPTL0(0x159) /* not used */
                    BTRAPTL0(0x15a) /* not used */
                    BTRAPTL0(0x15b) /* not used */
                    BTRAPTL0(0x15c) /* not used */
                    BTRAPTL0(0x15d) /* not used */
                    BTRAPTL0(0x15e) /* not used */
                    BTRAPTL0(0x15f) /* not used */
                    BTRAPTL0(0x160) /* not used */
                    BTRAPTL0(0x161) /* not used */
                    BTRAPTL0(0x162) /* not used */
                    BTRAPTL0(0x163) /* not used */
                    BTRAPTL0(0x164) /* not used */
                    BTRAPTL0(0x165) /* not used */
                    BTRAPTL0(0x166) /* not used */
                    BTRAPTL0(0x167) /* not used */
                    BTRAPTL0(0x168) /* not used */
                    BTRAPTL0(0x169) /* not used */
                    BTRAPTL0(0x16a) /* not used */
                    BTRAPTL0(0x16b) /* not used */
                    BTRAPTL0(0x16c) /* not used */
                    BTRAPTL0(0x16d) /* not used */
                    BTRAPTL0(0x16e) /* not used */
                    BTRAPTL0(0x16f) /* not used */
                    BTRAPTL0(0x170) /* not used */
                    BTRAPTL0(0x171) /* not used */
                    BTRAPTL0(0x172) /* not used */
                    BTRAPTL0(0x173) /* not used */
                    BTRAPTL0(0x174) /* not used */
                    BTRAPTL0(0x175) /* not used */
                    BTRAPTL0(0x176) /* not used */
                    BTRAPTL0(0x177) /* not used */
                    BTRAPTL0(0x178) /* not used */
                    BTRAPTL0(0x179) /* not used */
                    BTRAPTL0(0x17a) /* not used */
                    BTRAPTL0(0x17b) /* not used */
                    BTRAPTL0(0x17c) /* not used */
                    BTRAPTL0(0x17d) /* not used */
                    BTRAPTL0(0x17e) /* not used */
                    BTRAPTL0(0x17f) /* not used */

#define BTRAPS_TL0(x) BTRAPTL0(x) BTRAPTL0(x+1) BTRAPTL0(x+2) BTRAPTL0(x+3) \
                    BTRAPTL0(x+4) BTRAPTL0(x+5) BTRAPTL0(x+6) BTRAPTL0(x+7)

/* the following ones are reserved for future implementations */
tl0_resv180:        BTRAPS_TL0(0x180) BTRAPS_TL0(0x188) /* reserved */
                    BTRAPS_TL0(0x190) BTRAPS_TL0(0x198) /* reserved */
                    BTRAPS_TL0(0x1a0) BTRAPS_TL0(0x1a8) /* reserved */
                    BTRAPS_TL0(0x1b0) BTRAPS_TL0(0x1b8) /* reserved */
                    BTRAPS_TL0(0x1c0) BTRAPS_TL0(0x1c8) /* reserved */
                    BTRAPS_TL0(0x1d0) BTRAPS_TL0(0x1d8) /* reserved */
                    BTRAPS_TL0(0x1e0) BTRAPS_TL0(0x1e8) /* reserved */
                    BTRAPS_TL0(0x1f0) BTRAPS_TL0(0x1f8) /* reserved */



/*
 * Trap table for trap level TL > 0
 */
sparc64_ttable_tl1:
                    /* 0x000 reserved, hw reset */
                    BOOT_START
                    BTRAPTL1(0x001) /* power on reset */
                    BTRAPTL1(0x002) /* watchdog reset */
                    BTRAPTL1(0x003) /* externally initiated reset */
                    BTRAPTL1(0x004) /* software initiated reset */
                    BTRAPTL1(0x005) /* RED state exception */
                    BTRAPTL1(0x006) /* reserved */
                    BTRAPTL1(0x007) /* reserved */
                    BTRAPTL1(0x008) /* instruction access exception */
tl1_resv009:        BTRAPTL1(0x009) /* instruction access MMU miss */
                    /* 0x00a instruction access error */
tl1_iae:            TRAP_TL1(trap_instruction_access_error_tl1)
tl1_resv00b:        BTRAPTL1(0x00b) /* reserved */
                    BTRAPTL1(0x00c) /* reserved */
                    BTRAPTL1(0x00d) /* reserved */
                    BTRAPTL1(0x00e) /* reserved */
                    BTRAPTL1(0x00f) /* reserved */
                    /* 0x010 illegal instruction */
tl1_illinst:        TRAP_TL1(trap_illegal_instruction_tl1)
                    BTRAPTL1(0x011) /* privileged opcode (instruction) */
tl1_unimpldd:       BTRAPTL1(0x012) /* unimplemented LDD */
tl1_unimpstd:       BTRAPTL1(0x013) /* unimplemented STD */
tl1_resv014:        BTRAPTL1(0x014) /* reserved */
                    BTRAPTL1(0x015) /* reserved */
                    BTRAPTL1(0x016) /* reserved */
                    BTRAPTL1(0x017) /* reserved */
                    BTRAPTL1(0x018) /* reserved */
                    BTRAPTL1(0x019) /* reserved */
                    BTRAPTL1(0x01a) /* reserved */
                    BTRAPTL1(0x01b) /* reserved */
                    BTRAPTL1(0x01c) /* reserved */
                    BTRAPTL1(0x01d) /* reserved */
                    BTRAPTL1(0x01e) /* reserved */
                    BTRAPTL1(0x01f) /* reserved */
                    /* 0x020 floating point disabled */
tl1_fpdis:          TRAP_TL1(trap_fp_disabled)
                    BTRAPTL1(0x021) /* ieee 754 floating point exception */
                    BTRAPTL1(0x022) /* other floating point exception */
                    BTRAPTL1(0x023) /* tag overflow */
                    /* 0x024 clean window (4 slots = 32 instructions) */
tl1_cleanwin:       CLEAN_WINDOW
                    BTRAPTL1(0x028) /* division by zero */
tl1_iperr:          BTRAPTL1(0x029) /* internal processor error */
tl1_resv02a:        BTRAPTL1(0x02a) /* reserved */
                    BTRAPTL1(0x02b) /* reserved */
                    BTRAPTL1(0x02c) /* reserved */
                    BTRAPTL1(0x02d) /* reserved */
                    BTRAPTL1(0x02e) /* reserved */
                    BTRAPTL1(0x02f) /* reserved */
                    /* 0x030 data access exception */
tl1_dax:            TRAP_EXC(trap_data_accesss_exception_tl1)
tl1_damm:           BTRAPTL1(0x031) /* data access MMU miss */
                    BTRAPTL1(0x032) /* 0x032 data access error */
tl1_dap:            BTRAPTL1(0x033) /* data access protection */
                    /* 0x034 memory address not aligned */
tl1_mna:            TRAP_EXC(trap_mem_address_not_aligned_tl1)
                    BTRAPTL1(0x035) /* lddf memory address not aligned */
                    BTRAPTL1(0x036) /* stdf memory address not aligned */
                    BTRAPTL1(0x037) /* privileged action (eg: asi access) */
tl1_ldqfmna:        BTRAPTL1(0x038) /* ldqf memory address not aligned */
tl1_stqfmna:        BTRAPTL1(0x039) /* stqf memory address not aligned */
tl1_resv03a:        BTRAPTL1(0x03a) /* reserved */
                    BTRAPTL1(0x03b) /* reserved */
                    BTRAPTL1(0x03c) /* reserved */
                    BTRAPTL1(0x03d) /* reserved */
                    BTRAPTL1(0x03e) /* reserved */
                    BTRAPTL1(0x03f) /* reserved */
tl1_aderr:          BTRAPTL1(0x040) /* asynchronous data error (ECC) */
                    /* Interrupt Level  1 implementation dependent */
tl1_irq01:          TRAP_IRQ(irq_handler, 1)
                    /* Interrupt Level  2 sbus/VME lvl 1 */
tl1_irq02:          TRAP_IRQ(irq_handler, 2)
                    /* Interrupt Level  3 sbus/VME lvl 2 */
tl1_irq03:          TRAP_IRQ(irq_handler, 3)
                    /* Interrupt Level  4 onboard scsi */
tl1_irq04:          TRAP_IRQ(irq_handler, 4)
                    /* Interrupt Level  5 sbus/VME lvl 3 */
tl1_irq05:          TRAP_IRQ(irq_handler, 5)
                    /* Interrupt Level  6 onboard ethernet */
tl1_irq06:          TRAP_IRQ(irq_handler, 6)
                    /* Interrupt Level  7 sbus/VME lvl 4 */
tl1_irq07:          TRAP_IRQ(irq_handler, 7)
                    /* Interrupt Level  8 onboard video */
tl1_irq08:          TRAP_IRQ(irq_handler, 8)
                    /* Interrupt Level  9 sbus/VME lvl 5,module */
tl1_irq09:          TRAP_IRQ(irq_handler, 9)
                    /* Interrupt Level 10 TOD clock */
tl1_irq10:          TRAP_IRQ(irq_handler,10)
                    /* Interrupt Level 11 sbus/VME lvl 6,floppy */
tl1_irq11:          TRAP_IRQ(irq_handler,11)
                    /* Interrupt Level 12 serial i/o */
tl1_irq12:          TRAP_IRQ(irq_handler,12)
                    /* Interrupt Level 13 sbus/VME lvl 7, audio, X-call */
tl1_irq13:          TRAP_IRQ(irq_handler,13)
                    /* Interrupt Level 14 hi-res system timer */
tl1_irq14:          TRAP_IRQ(irq_handler,14)
                    /* Interrupt Level 15 asynchronous memory fault */
tl1_irq15:          TRAP_IRQ(irq_handler,15)
tl1_resv050:        BTRAPTL1(0x050) /* reserved */
                    BTRAPTL1(0x051) /* reserved */
                    BTRAPTL1(0x052) /* reserved */
                    BTRAPTL1(0x053) /* reserved */
                    BTRAPTL1(0x054) /* reserved */
                    BTRAPTL1(0x055) /* reserved */
                    BTRAPTL1(0x056) /* reserved */
                    BTRAPTL1(0x057) /* reserved */
                    BTRAPTL1(0x058) /* reserved */
                    BTRAPTL1(0x059) /* reserved */
                    BTRAPTL1(0x05a) /* reserved */
                    BTRAPTL1(0x05b) /* reserved */
                    BTRAPTL1(0x05c) /* reserved */
                    BTRAPTL1(0x05d) /* reserved */
                    BTRAPTL1(0x05e) /* reserved */
                    BTRAPTL1(0x05f) /* reserved */
/* Implementation dependent definitions and priorities. */
                    /* 0x060 interrupt vector */
tl1_ivec:           TRAP_TL1(vec_interrupt)
                    BTRAPTL1(0x061) /* physical address watchpoint */
                    BTRAPTL1(0x062) /* virtual address watchpoint */
                    BTRAPTL1(0x063) /* corrected ECC error */
                    BTRAPTL1(0x064) /* undefined */
                    BTRAPTL1(0x065) /* undefined */
                    BTRAPTL1(0x066) /* undefined */
                    BTRAPTL1(0x067) /* undefined */
                    /* 0x068 data access MMU miss (4-slots) */
tl1_damiss:
#include "mm/dtlb_miss.S"
                    /* 0x06c data access protection (4-slots) */
tl1_daprot:
#include "mm/dtlb_prot.S"
tl1_resv070:        BTRAPTL1(0x070) /* implementation dependent */
                    BTRAPTL1(0x071) /* implementation dependent */
                    BTRAPTL1(0x072) /* implementation dependent */
                    BTRAPTL1(0x073) /* implementation dependent */
                    BTRAPTL1(0x074) /* implementation dependent */
                    BTRAPTL1(0x075) /* implementation dependent */
                    BTRAPTL1(0x076) /* implementation dependent */
                    BTRAPTL1(0x077) /* implementation dependent */
                    BTRAPTL1(0x078) /* implementation dependent */
                    BTRAPTL1(0x079) /* implementation dependent */
                    BTRAPTL1(0x07a) /* implementation dependent */
                    BTRAPTL1(0x07b) /* implementation dependent */
                    BTRAPTL1(0x07c) /* implementation dependent */
                    BTRAPTL1(0x07d) /* implementation dependent */
                    BTRAPTL1(0x07e) /* implementation dependent */
                    BTRAPTL1(0x07f) /* implementation dependent */

/* spill/fill traps, all of these are 32 instructions in size. */
                    /* otherwin  = 0, wstate.normal = 0..7 spill, normal */
                    /* 0x080 ... 0x09c */
tl1_s0n:            TRAP_NORMAL_SPILL_0
tl1_s1n:            TRAP_NORMAL_SPILL_1
tl1_s2n:            TRAP_NORMAL_SPILL_2
tl1_s3n:            TRAP_NORMAL_SPILL_3
tl1_s4n:            TRAP_NORMAL_SPILL_4
tl1_s5n:            TRAP_NORMAL_SPILL_5
tl1_s6n:            TRAP_NORMAL_SPILL_6
tl1_s7n:            TRAP_NORMAL_SPILL_7
                    /* otherwin != 0, wstate.normal = 0..7 spill, other */
                    /* 0x0a0 ... 0x0bc */
tl1_s0o:            TRAP_OTHER_SPILL_0
tl1_s1o:            TRAP_OTHER_SPILL_1
tl1_s2o:            TRAP_OTHER_SPILL_2
tl1_s3o:            TRAP_OTHER_SPILL_3
tl1_s4o:            TRAP_OTHER_SPILL_4
tl1_s5o:            TRAP_OTHER_SPILL_5
tl1_s6o:            TRAP_OTHER_SPILL_6
tl1_s7o:            TRAP_OTHER_SPILL_7
                    /* otherwin  = 0, wstate.normal = 0..7  fill, normal */
                    /* 0x0c0 ... 0x0dc */
tl1_f0n:            TRAP_NORMAL_FILL_0
tl1_f1n:            TRAP_NORMAL_FILL_1
tl1_f2n:            TRAP_NORMAL_FILL_2
tl1_f3n:            TRAP_NORMAL_FILL_3
tl1_f4n:            TRAP_NORMAL_FILL_4
tl1_f5n:            TRAP_NORMAL_FILL_5
tl1_f6n:            TRAP_NORMAL_FILL_6
tl1_f7n:            TRAP_NORMAL_FILL_7
                    /* otherwin != 0, wstate.normal = 0..7  fill, other */
                    /* 0x0e0 ... 0x0fc */
tl1_f0o:            TRAP_OTHER_FILL_0
tl1_f1o:            TRAP_OTHER_FILL_1
tl1_f2o:            TRAP_OTHER_FILL_2
tl1_f3o:            TRAP_OTHER_FILL_3
tl1_f4o:            TRAP_OTHER_FILL_4
tl1_f5o:            TRAP_OTHER_FILL_5
tl1_f6o:            TRAP_OTHER_FILL_6
tl1_f7o:            TRAP_OTHER_FILL_7

#define BTRAPS_TL1(x) BTRAPTL1(x) BTRAPTL1(x+1) BTRAPTL1(x+2) BTRAPTL1(x+3) \
                    BTRAPTL1(x+4) BTRAPTL1(x+5) BTRAPTL1(x+6) BTRAPTL1(x+7)

/* no software traps in TL > 0 */
tl1_resv100:        BTRAPS_TL1(0x100) BTRAPS_TL1(0x108) /* not used */
                    BTRAPS_TL1(0x110) BTRAPS_TL1(0x118) /* not used */
                    BTRAPS_TL1(0x120) BTRAPS_TL1(0x128) /* not used */
                    BTRAPS_TL1(0x130) BTRAPS_TL1(0x138) /* not used */
                    BTRAPS_TL1(0x140) BTRAPS_TL1(0x148) /* not used */
                    BTRAPS_TL1(0x150) BTRAPS_TL1(0x158) /* not used */
                    BTRAPS_TL1(0x160) BTRAPS_TL1(0x168) /* not used */
                    BTRAPS_TL1(0x170) BTRAPS_TL1(0x178) /* not used */
/* the following ones are reserved for future implementations */
tl1_resv180:        BTRAPS_TL1(0x180) BTRAPS_TL1(0x188) /* reserved */
                    BTRAPS_TL1(0x190) BTRAPS_TL1(0x198) /* reserved */
                    BTRAPS_TL1(0x1a0) BTRAPS_TL1(0x1a8) /* reserved */
                    BTRAPS_TL1(0x1b0) BTRAPS_TL1(0x1b8) /* reserved */
                    BTRAPS_TL1(0x1c0) BTRAPS_TL1(0x1c8) /* reserved */
                    BTRAPS_TL1(0x1d0) BTRAPS_TL1(0x1d8) /* reserved */
                    BTRAPS_TL1(0x1e0) BTRAPS_TL1(0x1e8) /* reserved */
                    BTRAPS_TL1(0x1f0) BTRAPS_TL1(0x1f8) /* reserved */

  .global sparc64_ttable_end
sparc64_ttable_end:

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
