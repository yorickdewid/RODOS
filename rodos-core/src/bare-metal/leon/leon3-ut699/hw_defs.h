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
 * @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Copyright 2010 DLR
 *
 * @brief Constants: Usable from both C and Assembler
 *
 * There must be nothing but preprocessor stuff in this file, because
 * this is used from both C and Assembler.
 */

#ifndef RODOS_HW_LEON2_DEFS_H
#define RODOS_HW_LEON2_DEFS_H

#include "hw_config.h"
#include "hw_bitmagic.h"


/**
 * @defgroup hwMisc Miscelaneous defines
 */
/**@{*/
#define HW_SYS_PANIC_MSG_SIZEOF       200           ///< maximal length of panic error message
#define HW_SEC_RESET                  ".reset"      ///< section for reset code: start of ROM
#define HW_SEC_TRAP                   ".trap"       ///< section for trap table, start trap: 0x01
#define HW_SEC_STARTUP                ".boot"       ///< section for boot code: in ROM
#define HW_SEC_TRAP_TEXT              ".traptext"   ///< text section for trap handlers (max. 2MB)
#define HW_SEC_TEXT                   ".text"       ///< normal text section
#define HW_SEC_RODATA                 ".rodata"     ///< normal rodata section
#define HW_SEC_DATA                   ".data"       ///< normal data section
#define HW_SEC_BSS                    ".bss"        ///< normal bss section
#define HW_SEC_ROM_DATA               ".romdata"    ///< section for rodata in ROM
#define HW_SEC_SDRAM_DATA             ".sdramdata"  ///< section for data in SDRAM


/** Marker for (read-only!) data to be put into ROM.
 *
 * This kind of data is always read-only, of course, and it is
 * available during at boot time, because it does not need the memory
 * controller to be initialised. */
#define __ROM_DATA                   __attribute__((__section__(HW_SEC_ROM_DATA)))


/** Marker for data to be put into SDRAM
 *
 * Like data put into SRAM, this data is copied from ROM to RAM at boot time
 * and is then used from SDRAM. */
#define __SDRAM_DATA                 __attribute__((__section__(HW_SEC_SDRAM_DATA)))


/** Marker for 8-byte aligned data
 *
 * It is quite common on the Sparc architecture to require data to have 8-byte
 * alignment, so this is a macro to enforce that.  It can be used for types, too.
 */
#define __ALIGN8                     __attribute__((__aligned__(8)))

/**@}*/


/**
 * @defgroup hwOnChipReg APB On-Chip Registers
 *
 * See LEON2 Manual 7.1.
 */
/**@{*/
#define HW_BASE_OCR                       0x80000000 /**< Start of on-chip register area */
#define HW_OCR_MCFG1                      0x00       /**< Memory configuration register 1 */
#define HW_OCR_MCFG2                      0x04       /**< Memory configuration register 2 */
#define HW_OCR_MCFG3                      0x08       /**< Memory configuration register 3 */
#define HW_OCR_AHB_FAILING_ADDRESS        0x0C       /**< AHB Failing address register */
#define HW_OCR_AHB_STATUS                 0x10       /**< AHB status register */
#define HW_OCR_CACHE_CONTROL              0x14       /**< Cache control register */
#define HW_OCR_POWER_DOWN                 0x18       /**< Power-down register */
#define HW_OCR_WRITE_PROTECT_1            0x1C       /**< Write protection register 1 */
#define HW_OCR_WRITE_PROTECT_2            0x20       /**< Write protection register 2 */
#define HW_OCR_LEON_CONFIG                0x24       /**< LEON configuration register */
#define HW_OCR_TIMER_1_COUNTER            0x330       /**< Timer 1 counter register TODO JM GPTIMER 3*/
#define HW_OCR_TIMER_1_RELOAD             0x334       /**< Timer 1 reload register TODO JM GPTIMER 3*/
#define HW_OCR_TIMER_1_CONTROL            0x338      /**< Timer 1 control register TODO JM GPTIMER 3*/
#define HW_OCR_WATCHDOG_COUNTER           0x4C       /**< Watchdog register */
#define HW_OCR_TIMER_2_COUNTER            0x340       /**< Timer 2 counter register TODO JM GPTIMER 4*/
#define HW_OCR_TIMER_2_RELOAD             0x344       /**< Timer 2 reload register TODO JM GPTIMER 4*/
#define HW_OCR_TIMER_2_CONTROL            0x348       /**< Timer 2 control register TODO JM GPTIMER 4*/
#define HW_OCR_PRESCALER_COUNTER          0x300       /**< Prescaler counter register TODO JM*/
#define HW_OCR_PRESCALER_RELOAD           0x304       /**< Prescaler reload register TODO JM*/
#define HW_OCR_UART_1_DATA                0x100       /**< Uart 1 data register TODO JM*/
#define HW_OCR_UART_1_STATUS              0x104       /**< Uart 1 status register TODO JM*/
#define HW_OCR_UART_1_CONTROL             0x108       /**< Uart 1 control register TODO JM*/
#define HW_OCR_UART_1_SCALER              0x10C       /**< Uart 1 scaler register TODO JM*/
#define HW_OCR_UART_2_DATA                0x80       /**< Uart 2 data register */
#define HW_OCR_UART_2_STATUS              0x84       /**< Uart 2 status register */
#define HW_OCR_UART_2_CONTROL             0x88       /**< Uart 2 control register */
#define HW_OCR_UART_2_SCALER              0x8C       /**< Uart 2 scaler register */
#define HW_OCR_INTERRUPT_MASK_AND_PRIOR   0x240       /**< Interrupt mask and priority register TODO JM*/
#define HW_OCR_INTERRUPT_PENDING          0x204       /**< Interrupt pending register TODO JM*/
#define HW_OCR_INTERRUPT_FORCE            0x208       /**< Interrupt force register TODO JM*/
#define HW_OCR_INTERRUPT_CLEAR            0x20C       /**< Interrupt clear register TODO JM*/
#define HW_OCR_IO_PORT_IO                 0xA0       /**< I/O port input/output register */
#define HW_OCR_IO_PORT_DIRECTION          0xA4       /**< I/O port direction register */
#define HW_OCR_IO_PORT_INTERRUPT_CONFIG1  0xA8       /**< I/O port interrupt config. register */
#define HW_OCR_IO_PORT_INTERRUPT_CONFIG2  0xA8       /**< I/O port interrupt config. register */
#define HW_OCR_INTERRUPT_2_MASK           0xB0       /**< Secondary interrupt mask register */
#define HW_OCR_INTERRUPT_2_PENDING        0xB4       /**< Secondary interrupt pending register */
#define HW_OCR_INTERRUPT_2_FORCE          0xB8       /**< Secondary interrupt status register */
#define HW_OCR_INTERRUPT_2_CLEAR          0xBC       /**< Secondary interrupt clear register */
#define HW_OCR_DSU_UART_STATUS            0xC4       /**< DSU UART status register */
#define HW_OCR_DSU_UART_CONTROL           0xC8       /**< DSU UART control register */
#define HW_OCR_DSU_UART_SCALER            0xCC       /**< DSU UART scaler register */
#define HW_OCR_WRITE_PROTECT_1_START      0xD0       /**< Write Protection Start Address 1 */
#define HW_OCR_WRITE_PROTECT_1_END        0xD4       /**< Write Protection End   Address 1 */
#define HW_OCR_WRITE_PROTECT_2_START      0xD8       /**< Write Protection Start Address 2 */
#define HW_OCR_WRITE_PROTECT_2_END        0xDC       /**< Write Protection End   Address 2 */
#define HW_OCR_FT_AHB_RAM                 0x1000     /**< RAM Configuration Register */
/**@}*/


/**
 * @defgroup hwInterrupts Assignment of External Interrupts
 *
 * See LEON2 Manual 7.2.2
 *
 * Note: if these are changed, the arrangement of trap handlers in crt.S
 * has to be changed accordingly.
 *
 * These constants can be used with hwEnableInt() and hwDisableInt().
 */
/**@{*/
#define HW_INT_PCI           14
#define HW_INT_ETHERNET      12
#define HW_INT_DSU_TRACE     11
#define HW_INT_SECOND_INT    10
#define HW_INT_TIMER_2       9
#define HW_INT_TIMER_1       8
#define HW_INT_PARPORT_3     7
#define HW_INT_PARPORT_2     6
#define HW_INT_PARPORT_1     5
#define HW_INT_PARPORT_0     4
#define HW_INT_UART_1        3
#define HW_INT_UART_2        2
#define HW_INT_AHB_ERROR     1
/**@}*/


/**
 * @defgroup hwRegBits Bits in different registers
 *
 * These defines are suitable to use with HW_POS, HW_WIDTH, HW_BITS, and HW_EXTRACT
 * from hw_bitmagic.h.
 *
 * Each is a pair, the first is the start bit, the second is the bit width.
 */
/**@{*/
#define HW_PSR_CWP           ( 0, 5)
#define HW_PSR_ET            ( 5, 1)
#define HW_PSR_PS            ( 6, 1)
#define HW_PSR_S             ( 7, 1)
#define HW_PSR_PIL           ( 8, 4)
#define HW_PSR_EF            (12, 1)
#define HW_PSR_EC            (13, 1)
#define HW_PSR_ICC           (20, 4)
#define HW_PSR_VER           (24, 4)
#define HW_PSR_IMPL          (28, 4)
#define HW_PSR_ARCH          (24, 8)  /**< includes implementation and version */

#define HW_TBR_TT            ( 4, 8)

#define HW_MCFG1_PROM_WIDTH  ( 8, 2)

#define HW_TIMER_EN          ( 0, 1)
#define HW_TIMER_RL          ( 1, 1)
#define HW_TIMER_LD          ( 2, 1)
#define HW_TIMER_IE          ( 3, 1)

/** See LEON manual Section 7.5.6 */
#define HW_UART_CTRL_RE      ( 0, 1)
#define HW_UART_CTRL_TE      ( 1, 1)
#define HW_UART_CTRL_RI      ( 2, 1)
#define HW_UART_CTRL_TI      ( 3, 1)
#define HW_UART_CTRL_PS      ( 4, 1)
#define HW_UART_CTRL_PE      ( 5, 1)
#define HW_UART_CTRL_FL      ( 6, 1)
#define HW_UART_CTRL_LB      ( 7, 1)
#define HW_UART_CTRL_EC      ( 8, 1)

#define HW_UART_STAT_DR      ( 0, 1)
#define HW_UART_STAT_TS      ( 1, 1)
#define HW_UART_STAT_TH      ( 2, 1)
#define HW_UART_STAT_BR      ( 3, 1)
#define HW_UART_STAT_OV      ( 4, 1)
#define HW_UART_STAT_PE      ( 5, 1)
#define HW_UART_STAT_FE      ( 6, 1)

#define HW_UART_SCALER      ( 0, 11)

#define HW_IO_PORT_TXD1     (15,  1)
#define HW_IO_PORT_RTS1     (13,  1)
#define HW_IO_PORT_TXD2     (11,  1)
#define HW_IO_PORT_RTS2     ( 9,  1)

#define HW_WRITE_PROTECT_BP ( 1,  1)
#define HW_WRITE_PROTECT_US ( 1,  1)
#define HW_WRITE_PROTECT_SU ( 0,  1)
/**@}*/


/**
 * @defgroup hwInitVal Initial values for some registers
 */
/**@{*/
/**
 * Bits that must be restored in a task switch.
 */
#define HW_PSR_SWITCH_MASK       (HW_BITS(HW_PSR_PS)  | \
                                  HW_BITS(HW_PSR_ICC) | \
                                  HW_BITS(HW_PSR_PIL))

/**
 * PSR value for supervisor mode with disabled traps
 * 0: PIL, CWP, ICC, PS, ET
 * 1: S, EC, EF
 */
#define HW_INIT_PSR_SUPERVISOR  (HW_BITS(HW_PSR_S)  | \
                                 HW_BITS(HW_PSR_EC) | \
                                 HW_BITS(HW_PSR_EF))

#if (HW_INIT_PSR_SUPERVISOR & HW_PSR_SWITCH_MASK) != 0
#error "Assertion failed: user bits must be 0 in supervisor PSR init value."
#endif

/**
 * PSR value for normal user code
 * 0: PIL, CWP, ICC, PS, W
 * 1: ET,  EC,  EF
 */
#define HW_INIT_PSR_USER           (HW_BITS(HW_PSR_ET) | \
                                    HW_BITS(HW_PSR_EC) | \
                                    HW_BITS(HW_PSR_EF))

#if (HW_INIT_PSR_USER & HW_PSR_SWITCH_MASK) != 0
#error "Assertion failed: user bits must be 0 in user PSR init value."
#endif


/**
 * Init and reset values for the timer and watchdog registers.
 * See LEON2 Manual 7.4.2 */
#define HW_TIMER_COUNTER_MIN       0
#define HW_TIMER_COUNTER_MAX       HW_UMAX(24)

#define HW_INIT_TIMER_1_RELOAD     HW_TIMER_COUNTER_MAX  ///< will be reset by Timer::setInterval

#ifndef HW_INIT_TIMER_2_RELOAD
#define HW_INIT_TIMER_2_RELOAD     HW_TIMER_COUNTER_MAX  ///< maximize to get few interrupts
#endif

/**
 * Initially we do not set the enable bit so both timers are disabled.
 * HW_INIT_TIMER_1_CONTROL will be set in Timer::init().
 */
#define HW_INIT_TIMER_1_STARTUP    (HW_BITS(HW_TIMER_RL) | \
                                    HW_BITS(HW_TIMER_LD) | \
                                    HW_BITS(HW_TIMER_IE))
#define HW_INIT_TIMER_2_STARTUP    HW_INIT_TIMER_1_STARTUP

// TODO JM Added IE bit
#define HW_INIT_TIMER_1_CONTROL    (HW_BITS(HW_TIMER_EN) | \
                                    HW_BITS(HW_TIMER_RL) | \
                                    HW_BITS(HW_TIMER_LD) | \
                                    HW_BITS(HW_TIMER_IE))
/*#define HW_INIT_TIMER_1_CONTROL    (HW_BITS(HW_TIMER_EN) | \
                                    HW_BITS(HW_TIMER_RL) | \
                                    HW_BITS(HW_TIMER_LD)) */

#define HW_INIT_TIMER_2_CONTROL    HW_INIT_TIMER_1_CONTROL

#define HW_INIT_WATCHDOG_COUNTER   HW_CROP(                               \
                                     HW_TIMER_COUNTER_MIN,                \
                                     HW_TIMER_COUNTER_MAX,                \
                                     HW_CONFIG_WATCHDOG_BOOT_TIMEOUT - 1)

#define HW_RESET_WATCHDOG_COUNTER  HW_CROP(                          \
                                     HW_TIMER_COUNTER_MIN,           \
                                     HW_TIMER_COUNTER_MAX,           \
                                     HW_CONFIG_WATCHDOG_TIMEOUT - 1)

/**
 * For the prescaler init values, see LEON2 Manual 7.4.2 */
#define HW_PRESCALER_MIN           3
#define HW_PRESCALER_MAX           HW_UMAX(10)

#define HW_INIT_PRESCALER_RELOAD   HW_CROP(                          \
                                     HW_PRESCALER_MIN,               \
                                     HW_PRESCALER_MAX,               \
                                     HW_CONFIG_TIME_GRANULARITY - 1)

#define HW_INIT_PRESCALER_COUNTER  0
/**@}*/



/**
 * @defgroup hwUserTrapNo  Assignment of numbers of user traps.
 *
 * Note that you cannot simply change them here, because you must
 * re-arrange the trap handlers accordingly in crt.S.
 */
/**@{*/
#define HW_USER_TRAP_USER_BREAKPOINT     1
#define HW_USER_TRAP_DIVISION_BY_ZERO    2   ///< gcc runtime uses this in its math libs
#define HW_USER_TRAP_ERROR_MODE          5   ///< cause error mode and halt the CPU
#define HW_USER_TRAP_SAVE_CONTEXT        6   ///< initiate a task switch manually (Thread::yield)
#define HW_USER_TRAP_ENTER_TRAP_MODE     11  ///< enter supervisor mode with S=1,   ET=1, PIL=15
#define HW_USER_TRAP_LEAVE_TRAP_MODE     13  ///< return to old mode with    S=old, ET=1, PIL=0
#define HW_USER_TRAP_DISABLE_TRAPS       15  ///< enter supervisor mode with S=1,   ET=0
/**@}*/


/**
 * @defgroup hwContextOffset  Offsets and size of the context structure
 *
 * The offset is relative to the frame pointer of the corresponding
 * stack frame, i.e., negative.  Of source, HT_CTXT_SIZEOF is positive.
 * The offset is negative because that's how it will be used in crt.S.
 * Also, by moving backwards, each define contains the size of the
 * corresponding block (if we had moved forward, then the next define
 * would have contained the offset).
 * Note that the floating point vars are at the beginning of the frame,
 * because they need the largest alignment.  The SIZEOF will be aligned
 * to 8 bytes, too, just to get a valid stack pointer (although it
 * won't be used, because this is the last dead end stack frame).
 *
 * Read the following list backwards, as it is ordered by ascending
 * address.
 *
 * Note that G0 will not be written (because it's hardwired 0), but
 * FSR will be written there instead.
 *
 * The location for O6, which is also not written, because it's
 * our context pointer, is still unused.
 *
 * Always keep in sync with crt.S.
 */
/**@{*/
#define HW_CTXT_SIZEOF          HW_ALIGN_UP(8, -HW_CTXT_OFFSET_START)
#define HW_CTXT_OFFSET_START    (HW_CTXT_OFFSET_NPC_Y)
#define HW_CTXT_OFFSET_NPC_Y    (HW_CTXT_OFFSET_PSR_PC -      8)    ///< stored at once
#define HW_CTXT_OFFSET_PSR_PC   (HW_CTXT_OFFSET_O0     -      8)    ///< stored at once
#define HW_CTXT_OFFSET_O0       (HW_CTXT_OFFSET_G0     -  8 * 4)    ///< O6 is not stored
#define HW_CTXT_OFFSET_G0       (HW_CTXT_OFFSET_F0     -  8 * 4)    ///< G0 is not stored

#if HW_CONFIG_HAVE_FLOAT
#  define HW_CTXT_OFFSET_F0     (0                     - 32 * 4)
#else
#  define HW_CTXT_OFFSET_F0     (0                     -      0)
#endif

/* convenience definitions: */
#define HW_CTXT_OFFSET_O(_I)    (HW_CTXT_OFFSET_O0 + ((_I) * 4))
#define HW_CTXT_OFFSET_G(_I)    (HW_CTXT_OFFSET_G0 + ((_I) * 4))

#define HW_CTXT_OFFSET_F(_I)    (HW_CTXT_OFFSET_F0 + ((_I) * 4))

#define HW_CTXT_OFFSET_FSR      HW_CTXT_OFFSET_G(0)                 ///< use G0 location for FSR

#define HW_CTXT_OFFSET_NPC      (HW_CTXT_OFFSET_NPC_Y  + 0)
#define HW_CTXT_OFFSET_Y        (HW_CTXT_OFFSET_NPC_Y  + 4)

#define HW_CTXT_OFFSET_PSR      (HW_CTXT_OFFSET_PSR_PC + 0)
#define HW_CTXT_OFFSET_PC       (HW_CTXT_OFFSET_PSR_PC + 4)
/**@}*/


/**
 * @defgroup hwFrameOffset  Offsets and size of registers in a stack frame
 *
 * This is defined by the normal SPARC ABI: in each stack frame,
 * there is space for 16 registers to be saved.  They are
 * allocated relative to %sp.  So that's what these offsets define.
 */
/**@{*/
#define HW_FRAME_OFFSET_L0      (0)
#define HW_FRAME_OFFSET_I0      (HW_FRAME_OFFSET_L0 + 8 * 4)
#define HW_FRAME_SIZEOF         (HW_FRAME_OFFSET_I0 + 8 * 4)

#define HW_FRAME_OFFSET_L(_I)   (HW_FRAME_OFFSET_L0 + ((_I) * 4))
#define HW_FRAME_OFFSET_I(_I)   (HW_FRAME_OFFSET_I0 + ((_I) * 4))

#define HW_FRAME_OFFSET_FP      HW_FRAME_OFFSET_I(6)
/**@}*/

/**
 * @defgroup hwCacheControl Cache Control constants
 * See UT699 Manual 2.4.8
 */
/**@{*/

#define ASI_CCR     0x02

#define CCR_OFFSET  0x00

#define CCR_ICSE    0x00000003 /* Instruction Cache State: Enable */
#define CCR_DCSE    0x0000000C /* Data Cache State: Enable */
#define CCR_IF      0x00000010 /* Instruction Cache Freeze on Interrupt */
#define CCR_DF      0x00000020 /* Data Cache Freeze on Interrupt */
#define CCR_IB      0x00010000 /* Instruction Burst Fetch */
#define CCR_SN      0x00800000 /* D-Cache Snooping */

/**@}*/

#endif /* RODOS_HW_LEON2_CORE_H */
