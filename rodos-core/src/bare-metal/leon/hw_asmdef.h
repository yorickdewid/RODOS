/* -*- Mode: Assembler */

/**
 * @file
 * @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Copyright 2008-2010 DLR
 *
 * @brief Internal Macro definitions
 *
 * For assembler files, and some for both C and Assembler.
 *
 * This is exclusively for the implementation!  Never include this in .h files!
 * The names here will polute the global name space, as they do not have the
 * HW_ prefix!
 */

#ifndef RODOS_LEON_HW_ASMDEF_H
#define RODOS_LEON_HW_ASMDEF_H

#include "hw_defs.h"

/* Have a readable name for the zero register: */
#define zero %g0

#if 1
/* production: fast */
#  define NOP_WAIT3    nop ; nop ; nop
#  define NOP_WAIT2    nop ; nop
#  define NOP_WAIT1    nop
#  define NOP_WAIT0
#else
/* debug: really conservative */
#  define NOP_WAIT3    nop ; nop ; nop
#  define NOP_WAIT2    NOP_WAIT3
#  define NOP_WAIT1    NOP_WAIT3
#  define NOP_WAIT0    NOP_WAIT3
#endif


/**
 * According to the SPARC manual, some implementations may cause an interrupt
 * when setting PIL and ET=1 at the same time.  Therefore, we have a macro.
 * We assume that the LEON is not broken.
 *
 * The macro assumes that in __REG, PIL has the desired value and that ET=0.
 * The __XOR parameter is the additional immediate field of WR where you can
 * pass additional bits to modify in __REG.
 */
#if 1
/* good CPU */
#  define WR_PIL_AND_ET1(__REG, __XOR)  \
                                 wr __REG, (__XOR) + HW_BITS(HW_PSR_ET), %psr
#else
/* broken CPU */
#  define WR_PIL_AND_ET1(__REG, __XOR)  \
                                 wr __REG, (__XOR), %psr ; \
                                 NOP_WAIT3 ; \
                                 wr __REG, (__XOR) + HW_BITS(HW_PSR_ET), %psr
#endif


/**
 * CONST_HI() is just like %HI(), but only works on constant values and cannot
 * produce a relocation.  Same with CONST_LO() vs. %LO().
 * It has the advantage of allowing arithmetics beyond + and -, e.g.
 *   CONST_LO(FOO) | BAR
 * This is because
 *   %lo(FOO) | BAR
 * is wrong, as %lo() may produce a relocation, and there is no relocation to
 * perform | with the inserted value.  Unfortunately, the assembler issues an
 * error even if FOO is constant and there is no need for a relocation.
 * Thus CONST_LO() to overcome.
 */
#define CONST_HI(__X)   (((__X) >> 10) & HW_UMAX(22))
#define CONST_LO(__X)   ((__X) & HW_UMAX(10))

/**
 * Store a register window to a given location: */
#define SAVE_WINDOW(_stack)                         \
      std  %l0, [ _stack + HW_FRAME_OFFSET_L(0) ] ; \
      std  %l2, [ _stack + HW_FRAME_OFFSET_L(2) ] ; \
      std  %l4, [ _stack + HW_FRAME_OFFSET_L(4) ] ; \
      std  %l6, [ _stack + HW_FRAME_OFFSET_L(6) ] ; \
      std  %i0, [ _stack + HW_FRAME_OFFSET_I(0) ] ; \
      std  %i2, [ _stack + HW_FRAME_OFFSET_I(2) ] ; \
      std  %i4, [ _stack + HW_FRAME_OFFSET_I(4) ] ; \
      std  %i6, [ _stack + HW_FRAME_OFFSET_I(6) ]

/**
 * Load a register window from a given location: */
#define LOAD_WINDOW(_stack)                         \
      ldd  [ _stack + HW_FRAME_OFFSET_L(0) ], %l0 ; \
      ldd  [ _stack + HW_FRAME_OFFSET_L(2) ], %l2 ; \
      ldd  [ _stack + HW_FRAME_OFFSET_L(4) ], %l4 ; \
      ldd  [ _stack + HW_FRAME_OFFSET_L(6) ], %l6 ; \
      ldd  [ _stack + HW_FRAME_OFFSET_I(0) ], %i0 ; \
      ldd  [ _stack + HW_FRAME_OFFSET_I(2) ], %i2 ; \
      ldd  [ _stack + HW_FRAME_OFFSET_I(4) ], %i4 ; \
      ldd  [ _stack + HW_FRAME_OFFSET_I(6) ], %i6

#if HW_CONFIG_HAVE_WATCHDOG

  /** Macro to reset watchdog.  Argument are two free registers to use. */
#  define RESET_WATCHDOG(_VAL,_OCR_BASE)                   \
      sethi %hi(hwResetWatchdog), _VAL                   ; \
      ld    [ _VAL + %lo(hwResetWatchdog) ], _VAL        ; \
      set   HW_BASE_OCR, _OCR_BASE                       ; \
      st    _VAL, [ _OCR_BASE + HW_OCR_WATCHDOG_COUNTER ]

#else

#  define RESET_WATCHDOG(_VAL,_OCR_BASE)

#endif

// Space for a normal frame + y register + 7 global registers = 8 registers
#define TRAP_FRAME_SIZEOF     (HW_FRAME_SIZEOF + (4 * 8))

// Usually, one would index the stack for local variables using %fp
// with negative indices.  Other parts of the stack frame are indexed
// with positive indices to %sp.
//
// Due to a stack switch to kernel stack, %fp in the trampoline's
// reg window is user stack and cannot be used, while %sp is
// kernel stack.  So we index via %sp, with an offset of
// TRAP_FRAME_SIZEOF, our stack frame size.
#define FRAME_OFFSET_G0    %sp + (TRAP_FRAME_SIZEOF)  - (4 * 8)
#define FRAME_OFFSET_G(_N) FRAME_OFFSET_G0 + (4 * (_N))

/* Structure of UART state record */
#define UART_STATE_OCR_BASE         0
#define UART_STATE_GET_ERROR        4  // error message if something went wrong
#define UART_STATE_PUT_INSERT_PTR   8
#define UART_STATE_PUT_REMOVE_PTR  12
#define UART_STATE_GET_INSERT_PTR  16
#define UART_STATE_GET_REMOVE_PTR  20
#define UART_STATE_GET_BUFFER      24
#define UART_STATE_PUT_BUFFER      (UART_STATE_GET_BUFFER + HW_CONFIG_UART_BUFFER_ALLOC)
#define UART_STATE_SIZEOF          (UART_STATE_PUT_BUFFER + HW_CONFIG_UART_BUFFER_ALLOC)

// trap table base address
#define TBR_TBA_MASK        0xffffff
#define TBR_TBA_SHIFT       12
#define TBR_TBA_ADDRSHIFT   8 /* fixup the address by left shift */

// trap type
#define TBR_TT_MASK         0xff
#define TBR_TT_SHIFT        4

// processor state register
#define PSR_PIL 0xf00
#define PSR_ET  0x20

// offsets for the failure handler register file
// make sure to keep even register numbers aligned on an 8-byte boundary here
#define TF_L0       ( 0*4)
#define TF_L1       ( 1*4)
#define TF_L2       ( 2*4)
#define TF_G1       ( 3*4)
#define TF_G2       ( 4*4)
#define TF_G3       ( 5*4)
#define TF_G4       ( 6*4)
#define TF_G5       ( 7*4)
#define TF_G6       ( 8*4)
#define TF_G7       ( 9*4)
#define TF_I0       (10*4)
#define TF_I1       (11*4)
#define TF_I2       (12*4)
#define TF_I3       (13*4)
#define TF_I4       (14*4)
#define TF_I5       (15*4)
#define TF_I6       (16*4)
#define TF_I7       (17*4)
#define SF_L0       (18*4)
#define SF_L1       (19*4)
#define SF_L2       (20*4)
#define SF_L3       (21*4)
#define SF_L4       (22*4)
#define SF_L5       (23*4)
#define SF_L6       (24*4)
#define SF_L7       (25*4)
#define SF_I0       (26*4)
#define SF_I1       (27*4)
#define SF_I2       (28*4)
#define SF_I3       (29*4)
#define SF_I4       (30*4)
#define SF_I5       (31*4)
#define SF_I6       (32*4)
#define SF_I7       (33*4)
#define TF_TBR      (34*4)
#define TF_RET      (35*4)

#endif /* RODOS_LEON_HW_ASMDEF_H */
