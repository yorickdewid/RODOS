/* -*- Mode: C++ -*- */

/**
* @file
* @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2008-2010 DLR
*
* @brief Configuration constants
*
* This file is very lowlevel bit-stuff.  It's only purpose is to
* derive a number of configuration constants that the boot code
* stores in hardware configuration registers.  The computation of
* these constants depends on configuration options in hw_config.h.
*
* All constants must be compile-time (or link time) constants,
* because the boot code cannot invoke C routines at that early
* time.
*
* If you want to understand this, you *need* the LEON manual.
* At soon as you have the manual next to you, you will see that
* the constants are defined exactly along the lines of the
* hardware register documentation.
*
* The list below that initialises most hardware registers will
* be processed in order.  Registers may be initialised multiple
* times.  E.g. the interrupt controller ist first clear,
* configured and disabled, then the interrupt sources are
* configured, and finally, their interrupts are enabled.
*
* Also note: the following list must be completely constant!
* No function calls may be used to initialise this, because
* the global constructor functions are invoked only *after*
* this is read!
*/

#include <rodos.h>
#include "hw_core.h"

/* Memory Controller: see LEON2 Manual 8.10, 8.11, 8.12 */
__ROM_DATA unsigned const hwInitMCFG1=
      (((HW_CONFIG_IO_BUS_WIDTH) == 16 ? 0x1 :
        (HW_CONFIG_IO_BUS_WIDTH) == 32 ? 0x2 :
                                         0x0 )         << 27) |
      (HW_CROP(0, 1, HW_CONFIG_PROM_BUS_READY_ENABLE)  << 26) |
      (HW_CROP(0, 1, HW_CONFIG_PROM_BUS_ERROR_ENABLE)  << 25) |
      (HW_CROP(0,15, HW_CONFIG_IO_WAIT_STATES)         << 20) |
      (HW_CROP(0, 1, HW_CONFIG_IO_ENABLE)              << 19) |
      (HW_CROP(0, 1, HW_CONFIG_PROM_WRITE_ENABLE)      << 11) |
      (HW_CROP(0,15, HW_CONFIG_PROM_WRITE_WAIT_STATES) <<  4) |
      (HW_CROP(0,15, HW_CONFIG_PROM_READ_WAIT_STATES)  <<  0);

__ROM_DATA HWConfigRegVal const hwInitConfigRegVal[]= {

  /* Memory Controller, continued: see LEON2 Manual 8.10, 8.11, 8.12 */
  {
    address: HW_OCR(HW_OCR_MCFG2),
    value:
      (HW_CROP(0, 1, HW_CONFIG_SDRAM_REFRESH_ENABLE)   << 31) |
      (HW_CROP(0, 1, (HW_CONFIG_SDRAM_T_RP)  - 2)      << 30) |
      (HW_CROP(0, 7, (HW_CONFIG_SDRAM_T_RFC) - 3)      << 27) |
      (HW_CROP(0, 1, (HW_CONFIG_SDRAM_CAS)   - 2)      << 26) |

      (HW_CROP(0, 7,
        HW_INTEGER_LENGTH(
          HW_CONFIG_SDRAM_BANK_SIZE ) - 22)            << 23) |

      (((HW_CONFIG_SDRAM_COLUMN_SIZE) <=  256 ? 0x00 :
        (HW_CONFIG_SDRAM_COLUMN_SIZE) <=  512 ? 0x01 :
        (HW_CONFIG_SDRAM_COLUMN_SIZE) <= 1024 ? 0x02 :
        (HW_CONFIG_SDRAM_COLUMN_SIZE) <= 4096 ? 0x03 :
                                                0x00)  << 21) |

      (HW_CROP(0, 3, HW_CONFIG_SDRAM_COMMAND)          << 19) |

      (HW_CROP(0, 1, HW_CONFIG_DATA_BUS_WIDTH)          << 18) |

      (HW_CROP(0, 1, HW_CONFIG_SDRAM_ENABLE)           << 14) |
      (HW_CROP(0, 1, !HW_CONFIG_SRAM_ENABLE)           << 13) |   /* negated bit! */

      (HW_CROP(0, 15,
        HW_INTEGER_LENGTH(
          HW_CONFIG_SRAM_BANK_SIZE ) - 14)             <<  9) |

      (HW_CROP(0, 1, HW_CONFIG_SRAM_BUS_READY_ENABLE)  <<  7) |
      (HW_CROP(0, 1, (HW_CONFIG_SRAM_ENABLE_EDAC |
                      HW_CONFIG_SRAM_READ_MODIFY_WRITE)) <<  6) |

      (((HW_CONFIG_SRAM_BUS_WIDTH) == 16 ? 0x1 :
        (HW_CONFIG_SRAM_BUS_WIDTH) == 32 ? 0x2 :
                                           0x0)        <<  4) |

      (HW_CROP(0, 3, HW_CONFIG_SRAM_WRITE_WAIT_STATES) <<  2) |
      (HW_CROP(0, 3, HW_CONFIG_SRAM_READ_WAIT_STATES)  <<  0)
  },

#if 0
  // NOTE: if this is enabled, the grmon looses the DSU UART connection:
  // DCOM communication error, retrying ...

  /* LEON2 SPWRTC has a different MCFG3 which controlls EDAC support, which
   * requires more attention than just a single value.  See reset.S.
   */
  { /* Normal LEON2 MCFG3 register: */
    address: HW_OCR(HW_OCR_MCFG3),
    value:
      (HW_CROP(0, HW_UMAX(15),
         HW_CONFIG_SDRAM_REFRESH_COUNTER) << 12)
  },
#endif

#if HW_CONFIG_SRAM_HAVE_EDAC
  {
    address: HW_OCR(HW_OCR_FT_AHB_RAM),
    value:   (HW_CROP(0,1, HW_CONFIG_SRAM_ENABLE_EDAC) << 7)
  },
#endif

#if HW_CONFIG_ARCH != HW_ARCH_LEON3
  /* CCR controlled via ASI on LEON3 and therefore disabled. -AndreB */
  /* Cache Configuration Register: see LEON2 Manual 4.8 */
  {
    address: HW_OCR(HW_OCR_CACHE_CONTROL),
    value:
      (HW_CROP(0,1, HW_CONFIG_D_CACHE_SNOOP_ENABLE)  << 23) |
      (HW_CROP(0,1, HW_CONFIG_I_CACHE_BURST_ENABLE)  << 16) |
      (HW_CROP(0,1, HW_CONFIG_D_CACHE_FREEZE_ON_INT) <<  5) |
      (HW_CROP(0,1, HW_CONFIG_I_CACHE_FREEZE_ON_INT) <<  4) |
      ((HW_CONFIG_D_CACHE_ENABLE ? 0x3 : 0x0)        <<  2) |
      ((HW_CONFIG_I_CACHE_ENABLE ? 0x3 : 0x0)        <<  0)
  },
#endif

#if HW_CONFIG_HAVE_SECOND_INT
  /* Second Interrupt Controller: see LEON2 Manual 7.3 */
  {
    address: HW_OCR(HW_OCR_INTERRUPT_2_MASK),
    value:   0                              // disable all interrupts
  },
  {
    address: HW_OCR(HW_OCR_INTERRUPT_2_CLEAR),
    value:   ~0                             // clear all interrupt pending bits (again)
  },
#endif


  /* Interrupt Controller, Part 1: Initialisation: disable all interrupts.
   *
   * See LEON2 Manual 7.2
   *
   * We disable all interrupts now, then initialise the subsystems,
   * and then enable the interrupts.
   */
  {
    address: HW_OCR(HW_OCR_INTERRUPT_FORCE),
    value:   0                              // clear force bits
  },
  {
    address: HW_OCR(HW_OCR_INTERRUPT_PENDING),
    value:   0                              // clear pending state
  },
  {
    address: HW_OCR(HW_OCR_INTERRUPT_CLEAR),
    value:   HW_BITS((1,15))                // clear all interrupt pending bits (again)
  },

  {
    address: HW_OCR(HW_OCR_INTERRUPT_MASK_AND_PRIOR),
    value:
      ((0) << 16) |                         // priority: all low
      ((0) << 0)                            // disable add interrupts now.
  },


  /* ** Timer and Watchdog ** */
  /* Timer prescaler */
  {
    address: HW_OCR(HW_OCR_PRESCALER_RELOAD),
    value:   HW_INIT_PRESCALER_RELOAD
  },
  {
    address: HW_OCR(HW_OCR_PRESCALER_COUNTER),
    value:   HW_INIT_PRESCALER_COUNTER
  },

  /* Timer 1, the preemption timer */
  {
    address: HW_OCR(HW_OCR_TIMER_1_RELOAD),
    value:   HW_INIT_TIMER_1_RELOAD
  },
  {
    address: HW_OCR(HW_OCR_TIMER_1_CONTROL),
    value:   HW_INIT_TIMER_1_STARTUP
  },

  /* Timer 2, the sys time simple counter
  {
    address: HW_OCR(HW_OCR_TIMER_2_RELOAD),
    value:   HW_INIT_TIMER_2_RELOAD
  },
  {
    address: HW_OCR(HW_OCR_TIMER_2_CONTROL),
    value:   HW_INIT_TIMER_2_STARTUP
  },
  */

#if HW_CONFIG_HAVE_WATCHDOG
  /* Init Watchdog
   * It always runs and is reset at boot time to TIMER_COUNTER_MAX).
   * We set the desired timeout now. */
  {
    address: HW_OCR(HW_OCR_WATCHDOG_COUNTER),
    value:
      HW_CROP(
        HW_TIMER_COUNTER_MIN,
        HW_TIMER_COUNTER_MAX,
        HW_CONFIG_WATCHDOG_BOOT_TIMEOUT)
  },
#endif

  /* ** I/O Port direction ** */
  {
    address: HW_OCR(HW_OCR_IO_PORT_DIRECTION),
    value:
      HW_BITS(HW_IO_PORT_TXD1) |
      HW_BITS(HW_IO_PORT_RTS1) |
      HW_BITS(HW_IO_PORT_TXD2) |
      HW_BITS(HW_IO_PORT_RTS2)
  },

#if 0
  // NOTE: if this is enabled, the grmon looses the DSU UART connection:
  // DCOM communication error, retrying ...

  {
    address: HW_OCR(HW_OCR_IO_PORT_INTERRUPT_CONFIG1),
    value:   0
  },
  {
    address: HW_OCR(HW_OCR_IO_PORT_INTERRUPT_CONFIG2),
    value:   0
  },
#endif

  /* ** UARTs ** */
  /* Switch UARTs off at boot time.  They will be enabled by hwUARTConfigure(). */
  /* UART 1 */
  {
    address: HW_OCR(HW_OCR_UART_1_SCALER),
    value:
      HW_CROP(
        0,
        HW_BITS(HW_UART_SCALER),
        (unsigned)HW_DIV_ROUND(
          (HW_CONFIG_UART_EXTERNAL_CLOCK_HZ > 0 ?
            HW_CONFIG_UART_EXTERNAL_CLOCK_HZ
          : HW_CONFIG_SYS_CLOCK_HZ
          ),
          (8 * HW_CONFIG_UART_BAUD)
        ) - 1
      )
  },
  {
    address: HW_OCR(HW_OCR_UART_1_CONTROL),
    value:
      HW_BITS(HW_UART_CTRL_RE) |
      HW_BITS(HW_UART_CTRL_TE) |
      HW_BITS(HW_UART_CTRL_RI) |
      HW_BITS(HW_UART_CTRL_TI) |
      ((HW_CONFIG_UART_PARITY & 1)          ? HW_BITS(HW_UART_CTRL_PS) : 0) |
      (HW_CONFIG_UART_PARITY                ? HW_BITS(HW_UART_CTRL_PE) : 0) |
      (HW_CONFIG_UART_EXTERNAL_CLOCK_HZ > 0 ? HW_BITS(HW_UART_CTRL_EC) : 0) |
      (HW_CONFIG_UART_FLOW_CONTROL          ? HW_BITS(HW_UART_CTRL_FL) : 0) |
      (HW_CONFIG_UART_LOOP_BACK             ? HW_BITS(HW_UART_CTRL_LB) : 0)
  },
  {
    address: HW_OCR(HW_OCR_UART_1_STATUS),
    value:   0
  },

  /* UART 2 */
  {
    address: HW_OCR(HW_OCR_UART_2_SCALER),
    value:
      HW_CROP(
        0,
        HW_BITS(HW_UART_SCALER),
        (unsigned)HW_DIV_ROUND(
          (HW_CONFIG_UART_EXTERNAL_CLOCK_HZ > 0 ?
            HW_CONFIG_UART_EXTERNAL_CLOCK_HZ
          : HW_CONFIG_SYS_CLOCK_HZ
          ),
          (8 * HW_CONFIG_UART_BAUD)
        ) - 1
      )
  },
  {
    address: HW_OCR(HW_OCR_UART_2_CONTROL),
    value:
      HW_BITS(HW_UART_CTRL_RE) |
      HW_BITS(HW_UART_CTRL_TE) |
      HW_BITS(HW_UART_CTRL_RI) |
      HW_BITS(HW_UART_CTRL_TI) |
      ((HW_CONFIG_UART_PARITY & 1)          ? HW_BITS(HW_UART_CTRL_PS) : 0) |
      (HW_CONFIG_UART_PARITY                ? HW_BITS(HW_UART_CTRL_PE) : 0) |
      (HW_CONFIG_UART_EXTERNAL_CLOCK_HZ > 0 ? HW_BITS(HW_UART_CTRL_EC) : 0) |
      (HW_CONFIG_UART_FLOW_CONTROL          ? HW_BITS(HW_UART_CTRL_FL) : 0) |
      (HW_CONFIG_UART_LOOP_BACK             ? HW_BITS(HW_UART_CTRL_LB) : 0)
  },
  {
    address: HW_OCR(HW_OCR_UART_2_STATUS),
    value:   0
  },

  /* Interrupt Controller, Part 2: Finally enable interrupts: */
  {
    address: HW_OCR(HW_OCR_INTERRUPT_CLEAR),
    value:   HW_BITS((1,15))                // clear all interrupt pending bits (again)
  },
  {
    address: HW_OCR(HW_OCR_INTERRUPT_MASK_AND_PRIOR),
    value:
      ((0) << 16) |                         // priority: all low
      ((
         (1 << HW_INT_TIMER_1) |            // enable both timers now
         (1 << HW_INT_UART_1)  |            // enable ints from UARTs
         (1 << HW_INT_UART_2)               // (the transceivers are still disabled)
       ) << 0)                              // enable bits start at bit 1 for interrupt 1
  },


  { NULL, 0 }, // terminator
};


/* Watchdog reset value */
#if HW_CONFIG_HAVE_WATCHDOG
__ROM_DATA unsigned const hwResetWatchdog= HW_RESET_WATCHDOG_COUNTER;
#endif
