/* -*- Mode: C++ -*- */

// NOTE: <BSP>/hw_boardconfig.h may override these global defaults.

/**
 * @file
 * @author Henrik Theiling, AbsInt Angewandte Informatik GmbH
 *
 * Copyright 2010 DLR
 *
 * @brief Configuration of processor and board variant.
 *
 * There must be nothing but preprocessor stuff in this file, because
 * this is used from both C and Assembler.
 *
 * All of the configuration can also be set on the command line in
 * leon2-vars-envs script, too, because all defines are
 * ifndef-protected.
 */

#ifndef RODOS_HW_LEON2_CONFIG_H
#define RODOS_HW_LEON2_CONFIG_H

#include "params.h"

/**
 * Some conversion functions for easier specification
 * Do not change the values, these are functions.  They can be used
 * to configure options thay require clock tick counts by specifying
 * seconds.
 *
 * If the denominator is float/double, the result will be float/double,
 * otherwise it will be integer.
 */
#define HW_DIV_FLOOR(_NUM,_DENOM)        (((long long)(_NUM)) / (_DENOM))
#define HW_DIV_CEIL(_NUM,_DENOM)         HW_DIV_FLOOR(((long long)(_NUM)) + ((_DENOM)-1), _DENOM)
#define HW_DIV_ROUND(_NUM,_DENOM)        HW_DIV_FLOOR(((long long)(_NUM)) + ((_DENOM)/2), _DENOM)


/**
 * @defgroup hwArchID Architecture IDs.
 * Do not change the values, there are constants to be used to configure
 * HW_CONFIG_ARCH.  The hwConfigArch variable also holds values of this
 * kind.
 */
/**@{*/

#define HW_VENDOR_LEON                    0xf

/** The normal LEON2 professor. */
#define HW_ARCH_LEON2                     ((HW_VENDOR_LEON << 4) | 0x2)

/** The normal LEON3 professor. */
#define HW_ARCH_LEON3                     ((HW_VENDOR_LEON << 4) | 0x3)

/** The special LEON2-FT professor found on an Gaisler SPWRTC board. */
#define HW_ARCH_LEON2_SPWRTC              0x00

/**@}*/


/**
 * @defgroup hwConfigGen Configuration: General Hardware
 *
 * Some of this could be queried from the configuration word, but the
 * code would be larger, more complex, and slower then.
 *
 * We require two timers to be present, so this cannot be configured here.
 * We use one for the preemption timer and one for maintaining global time.
 */
/**@{*/

#include "hw_boardconfig.h"


/**
 * Which exact CPU type is used.  See the \ref hwArchID "HW_ARCH_* constants". */
#ifndef HW_CONFIG_ARCH
#define HW_CONFIG_ARCH                    HW_ARCH_LEON3
#endif

/** Number of register windows of the CPU.  Possible value: 2..32 */
#ifndef HW_CONFIG_REG_WINDOWS
#define HW_CONFIG_REG_WINDOWS             8
#endif

/** System clock rate.  This is a float that will be rounded to int, the unit is Hertz (1/s). */
#ifndef HW_CONFIG_SYS_CLOCK_HZ
#define HW_CONFIG_SYS_CLOCK_HZ            (50e6)
#endif

/** bool: There's floating point unit.
 * If enabled, the task switching code will save and restore the floating point state. */
#ifndef HW_CONFIG_HAVE_FLOAT
#define HW_CONFIG_HAVE_FLOAT              1
#endif

/** bool: There's secondary interrupt controller.
 * If enabled, the boot code will initialise the secondary interrupt controller.
 */
#ifndef HW_CONFIG_HAVE_SECOND_INT
#define HW_CONFIG_HAVE_SECOND_INT         1
#endif

/** bool: Enable this if you want automatic watchdog timer resets
 * for every task switch.  If you do disable this, you can use the
 * watchdog timer, if present in hardware, for your own purposes,
 * e.g. to supervise a very important task instead of the task
 * switching itself.
 */
#ifndef HW_CONFIG_HAVE_WATCHDOG
#define HW_CONFIG_HAVE_WATCHDOG           1
#endif

/** bool: The hardware has a pair of advanced write protection regs.
 * If enabled, the boot code will use the first of the two registers
 * to protect the read-only parts of the binary image when they are
 * copied to RAM.  This includes the code and the read-only data.
 *
 * The second pair of write protect registers will not be used.
 *
 * The normal write protect registers that a normal LEON2 cpu provides
 * will not be used by this implementation, because its protection
 * capabilities are too coarse-grained.
 */
#ifndef HW_CONFIG_HAVE_EXT_WRITE_PROTECT
#define HW_CONFIG_HAVE_EXT_WRITE_PROTECT  0
#endif
/**@}*/


/**
 * @defgroup hwConfigPref Configuration: General Preferences
 *
 * This lists settings that have nothing to do with hardware
 * configuration, but are general preferences that may be
 * chosen the way it suites your application best.
 */
/**@{*/

/** bool: Whether the whole software image should be in RAM (value 1),
 *	or in ROM (value 0)
 */
#ifndef HW_CONFIG_RAMIMAGE
#define HW_CONFIG_RAMIMAGE				1
#endif

/** bool: Whether .text is also moved to RAM => slower boot time,
 * faster execution afterwards.
 */
#ifndef HW_CONFIG_CODE_IN_RAM
#define HW_CONFIG_CODE_IN_RAM             1
#endif


/** bool: Whether to put the trap table and low-level service routines in RAM.
 *
 * This only has an effect if HW_CONFIG_CODE_IN_RAM is enabled.
 *
 * CAUTION: This might cause a lock-out for grmon(!) on an SPWRTC board for
 * unknown reasons, and you will need to stop the CPU from booting somehow
 * before being able to regain access, e.g. by flipping PIO[2], so be
 * very careful!  Gaisler has been contacted about this issue.
 */
#ifndef HW_CONFIG_TRAPS_IN_RAM
#define HW_CONFIG_TRAPS_IN_RAM            0
#endif


/** bool: Whether read-only data is also moved to RAM => slower boot time,
 * faster execution afterwards.
 */
#ifndef HW_CONFIG_RODATA_IN_RAM
#define HW_CONFIG_RODATA_IN_RAM           1
#endif


/** bool: Whether to check that the CPU is of the kind set with HW_CONFIG_ARCH.
 * Note that the LEON2 on a SPWRTC board has a different ID from a normal LEON2.
 * So this may be switched off for debugging, because the hardware is different
 * from the LEON2 simulator.
 */
#ifndef HW_CONFIG_CHECK_ARCH
#define HW_CONFIG_CHECK_ARCH              1
#endif

/** bool: Check for expected hardware state and responsiveness.
 * These are a few very simple tests at boot-up to see whether the
 * configuration settings match the hardware and whether the hardware
 * responds as expected.  This also include a test whether RAM
 * is writable, because with the wrong SRAM/SDRAM/EDAC settings,
 * this may go wrong.
 */
#ifndef HW_CONFIG_SANITY_CHECKS
#define HW_CONFIG_SANITY_CHECKS           1
#endif

/** bool: Enable EDAC support where possible.
 * This mainly effects the EDAC settings for the RAM, because we
 * never switch off EDAC checking of the register file (which is enabled
 * by default on the LEON CPUs that support that) or the PROM (which is
 * configured with the PIO[2] DIP switch on an SPWRTC board).  For RAM,
 * the EDAC checking must be enabled during boot-up, and this setting
 * will do that, if EDAC support is available.
 *
 * Also see the HW_CONFIG_SRAM_HAVE_EDAC setting.
 */
#ifndef HW_CONFIG_ENABLE_EDAC
#define HW_CONFIG_ENABLE_EDAC             1
#endif

/** Size of boot stack.  This is currently only used during boot time,
 * just as the name suggests.  The scheduler uses its own stack, so
 * after the first entrance to a task, the boot stack will never be used
 * again.
 */
#ifndef HW_CONFIG_STACK_SIZE
#define HW_CONFIG_STACK_SIZE              DEFAULT_STACKSIZE
#endif
/**@}*/


/**
 * @defgroup hwConfigPROM Configuration: PROM
 *
 * See Leon2 XST User's Manual 8.10 for configuration information.
 *
 * The PROM bus width will be inferred from the PIO[1:0] bits at
 * boot time: the hardware initialises them correctly.
 */
/**@{*/

/** The size of the PROM in bytes. This will be used in the linker script
 * to check that the code fits into memory. */
#ifndef HW_CONFIG_PROM_SIZE
#define HW_CONFIG_PROM_SIZE               0x20000000
#endif

/** Access time for PROM in Hz = 1/s.  This will be used to
 * compute the right number of PROM wait states. */
#ifndef HW_CONFIG_PROM_ACCESS_HZ
#define HW_CONFIG_PROM_ACCESS_HZ          (1e9 / 115)
#endif

/** May be 0..15.  By default, this will be automatically computed
 * from  the HW_CONFIG_PROM_ACCESS_HZ setting.
 *
 * Note that you typically do not want to write into the PROM
 * during execution, and that this port explicitly write-protects
 * the PROM in the memory controller.
 */
#ifndef HW_CONFIG_PROM_WRITE_WAIT_STATES
#define HW_CONFIG_PROM_WRITE_WAIT_STATES  ((unsigned)HW_DIV_CEIL( \
                                            HW_CONFIG_SYS_CLOCK_HZ, \
                                            HW_CONFIG_PROM_ACCESS_HZ))
#endif

/** May be 0..15.  By default, this is the same as the write wait
 * state setting. */
#ifndef HW_CONFIG_PROM_READ_WAIT_STATES
#define HW_CONFIG_PROM_READ_WAIT_STATES   HW_CONFIG_PROM_WRITE_WAIT_STATES
#endif

/** boolean: See the LEON documentation for details on this setting. */
#ifndef HW_CONFIG_PROM_BUS_READY_ENABLE
#define HW_CONFIG_PROM_BUS_READY_ENABLE   0
#endif                                                        

/** boolean: See the LEON documentation for details on this setting. */
#ifndef HW_CONFIG_PROM_BUS_ERROR_ENABLE
#define HW_CONFIG_PROM_BUS_ERROR_ENABLE   0
#endif
/**@}*/


/**
 * @defgroup hwConfigSRAM Configuration: SRAM
 *
 * See Leon2 XST User's Manual 8.11 for configuration information.
 */
/**@{*/

/** Access width of the SRAM BUS in number of bits: may be 8, 16, 32. */
#ifndef HW_CONFIG_SRAM_BUS_WIDTH
#define HW_CONFIG_SRAM_BUS_WIDTH          32
#endif

/** SRAM size in bytes.  Will be used in the linker script to check that
 * the program fits into memory.  In case it doesn't, you might want
 * to move code and/or read-only data to ROM. */
#ifndef HW_CONFIG_SRAM_SIZE
#define HW_CONFIG_SRAM_SIZE               (2 * 1024*1024)
#endif

/** May be 1, 2, 4, 8.  Please consult the hardware documentation
 * for the right setting. */
#ifndef HW_CONFIG_SRAM_BANKS
#define HW_CONFIG_SRAM_BANKS              1
#endif

/** In bytes.  The correct value can be found in the hardware documentation. */
#ifndef HW_CONFIG_SRAM_BANK_SIZE
#define HW_CONFIG_SRAM_BANK_SIZE          (HW_CONFIG_SRAM_SIZE / HW_CONFIG_SRAM_BANKS)
#endif

/** boolean:  See the LEON documentation for details on this setting. */
#ifndef HW_CONFIG_SRAM_BUS_READY_ENABLE
#define HW_CONFIG_SRAM_BUS_READY_ENABLE   0
#endif

/** boolean: If enabled, enforces 32-bit accesses to RAM only, instead
 * of specially masked accesses for sub-word accesses.
 *
 * See the LEON documentation for more details on this setting.
 *
 * Note that if EDAC checking is enabled for RAM, this setting will be
 * ignored, but will automatically be enabled, because only then the
 * EDAC protection works correctly.
 */
#ifndef HW_CONFIG_SRAM_READ_MODIFY_WRITE
#define HW_CONFIG_SRAM_READ_MODIFY_WRITE  0
#endif

/** May be 0..3: number of write wait states for RAM access. */
#ifndef HW_CONFIG_SRAM_WRITE_WAIT_STATES
#define HW_CONFIG_SRAM_WRITE_WAIT_STATES  0
#endif

/** May be 0..3: number of read wait states for RAM access. */
#ifndef HW_CONFIG_SRAM_READ_WAIT_STATES
#define HW_CONFIG_SRAM_READ_WAIT_STATES   0
#endif

/** May be 0 or 1: The hardware has EDAC support for AHB RAM.
 *
 * EDAC support can be enabled with the HW_CONFIG_ENABLE_EDAC setting.
 */
#ifndef HW_CONFIG_SRAM_HAVE_EDAC
#define HW_CONFIG_SRAM_HAVE_EDAC          0
#endif
/**@}*/


/**
 * @defgroup hwConfigIO Configuration: I/O
 *
 * See Leon2 XST User's Manual 8.10 for configuration information.
 */
/**@{*/

/** Bits: may be 8, 16, 32.
 * See the hardware manual for the correct setting. */
#ifndef HW_CONFIG_IO_BUS_WIDTH
#define HW_CONFIG_IO_BUS_WIDTH            32
#endif

/** May be 0..15.
 * Seet the hardware manual for the correct setting. */
#ifndef HW_CONFIG_IO_WAIT_STATES
#define HW_CONFIG_IO_WAIT_STATES          7
#endif
/**@}*/


/**
 * @defgroup hwConfigSDRAM Configuration: SDRAM
 *
 * See Leon2 XST User's Manual 8.11 for configuration information.
 *
 * HW_CONFIG_SDRAM_BANKS is not used directly, but used to derive
 * HW_CONFIG_SDRAM_BANK_SIZE, which you may choose to set directly.
 */
/**@{*/
#ifndef HW_CONFIG_SDRAM_SIZE
#define HW_CONFIG_SDRAM_SIZE              0    ///< in bytes
#endif

#ifndef HW_CONFIG_SDRAM_CAS
#define HW_CONFIG_SDRAM_CAS               2    ///< may be 2 or 3
#endif

#ifndef HW_CONFIG_SDRAM_T_RP_HZ
#define HW_CONFIG_SDRAM_T_RP_HZ           HW_DIV_FLOOR(1e9, 20.0)   ///< float: in hertz: 1/s
#endif

#ifndef HW_CONFIG_SDRAM_T_RFC_HZ
#define HW_CONFIG_SDRAM_T_RFC_HZ          HW_DIV_FLOOR(1e9, 66.0)   ///< float: in hertz: 1/s
#endif

#ifndef HW_CONFIG_SDRAM_BANKS
#define HW_CONFIG_SDRAM_BANKS             1    /**< may be 1, 2, 4 */
#endif

#ifndef HW_CONFIG_SDRAM_BANK_SIZE
#define HW_CONFIG_SDRAM_BANK_SIZE         (HW_CONFIG_SDRAM_SIZE / HW_CONFIG_SDRAM_BANKS) /**< in bytes */
#endif

#ifndef HW_CONFIG_SDRAM_COLUMN_SIZE
#define HW_CONFIG_SDRAM_COLUMN_SIZE       512  /**< in bytes: may be 256, 512, 1024, 2048 or 4096 */
#endif

#ifndef HW_CONFIG_SDRAM_REFRESH_HZ
#define HW_CONFIG_SDRAM_REFRESH_HZ        HW_DIV_CEIL(1e6, 7.8) ///< float: in hertz: 1/s
#endif
/**@}*/


/**
 * @defgroup hwConfigCache Configuration: Cache
 *
 * See Leon2 XST User's Manual 4.8 for configuration information.
 */
/**@{*/
#ifndef HW_CONFIG_I_CACHE_ENABLE
#define HW_CONFIG_I_CACHE_ENABLE          1  /**< boolean */
#endif

#ifndef HW_CONFIG_I_CACHE_FREEZE_ON_INT
#define HW_CONFIG_I_CACHE_FREEZE_ON_INT   0  /**< boolean */
#endif

#ifndef HW_CONFIG_I_CACHE_BURST_ENABLE
#define HW_CONFIG_I_CACHE_BURST_ENABLE    1  /**< boolean */
#endif

#ifndef HW_CONFIG_D_CACHE_ENABLE
#define HW_CONFIG_D_CACHE_ENABLE          1  /**< boolean */
#endif

#ifndef HW_CONFIG_D_CACHE_FREEZE_ON_INT
#define HW_CONFIG_D_CACHE_FREEZE_ON_INT   0  /**< boolean */
#endif

#ifndef HW_CONFIG_D_CACHE_SNOOP_ENABLE
#define HW_CONFIG_D_CACHE_SNOOP_ENABLE    1  /**< boolean */
#endif
/**@}*/


/**
 * @defgroup hwConfigTimer Configuration: Timer and Watchdog
 *
 * See Leon2 Manual 7.4 for details
 *
 * To get exact timings, see that the following quotients are integer:
 *
 *   TIMER_CLOCK_HZ = (SYS_CLOCK_HZ / TIME_GRANULARITY_HZ)
 *   1e9 / TIMER_CLOCK_HZ
 *
 * Otherwise, there might be rounding in both the system clock (relative
 * time since boot) and in preemption generation.
 */
/**@{*/

/** In Hertz: 1/s: this configures the prescaler for all timers.
 * So this setting defines the smallest unit of time usable
 * for the preemption timer, the system time, and the watchdog
 * timer.  There is no advantage of making this slow.  All timers
 * are wide enough to cope with a prescaler at maximum speed.
 *
 * The maximum frequency for this setting is system clock / 3.
 * If you set this faster, it will be corrected to that speed.
 */
#ifndef HW_CONFIG_TIME_GRANULARITY_HZ
#define HW_CONFIG_TIME_GRANULARITY_HZ     (1 * 1000 * 1000)
#endif


/**
 * The watchdog is reset whenever the kernel switches into a task
 * (i.e., in __asmSwitchToContext).
 *
 * This happens at least as many times as the preemption counter enforces,
 * configured with the PARAM_TIMER_INTERVAL setting in params.h
 *
 * So this should be longer the ticker clock period, otherwise the watchdog
 * will trigger immediately.
 *
 * Note: for system boot time, there's a different watchdog setting, because
 * boot-up might take longer.  It is not expected to take long, but it's the
 * longest time the interrupts are disabled, so better safe than sorry.
 *
 * In case you set the values too large, they will be culled.  The maximum
 * watchdog timeout is 2^24 timer ticks (around 16 million).  E.g. at a
 * frequency of 50MHz and a TIME_GRANULARITY of 4, 2^24 / (50e6 / 4) seconds,
 * i.e., about 1.34s.
 *
 * In hertz: 1/s.
 */
#ifndef HW_CONFIG_WATCHDOG_TIMEOUT_HZ
#define HW_CONFIG_WATCHDOG_TIMEOUT_HZ        10
#endif

/** In hertz: 1/s: the initial setting of the watchdog timeout
 * at boot time.  In case you have a lot of initialisations at
 * boot time, this should be higher than than the normal watchdog
 * timeout. */
#ifndef HW_CONFIG_WATCHDOG_BOOT_TIMEOUT_HZ
#define HW_CONFIG_WATCHDOG_BOOT_TIMEOUT_HZ   1
#endif
/**@}*/


/**
 * @defgroup hwConfigUART Configuration: UART
 */
/**@{*/

/**
 * Size of the put and get buffers used by the hardware driver.
 *
 * The transmission is also done in the interrupt that the UART
 * generates, so transceive buffers are used to drive the UART
 * as quickly as possible.
 *
 * This setting must be a power of 2 (or better: it will be rounded
 * up to the next power of two anyway), and it must be at least 2.
 *
 * Without flow control, this should be large enough to receive
 * characters while other task than the receiving one run, because
 * otherwise you'll get a buffer overrun.  Of course this will only
 * happen if you switch off flow control, because with flow control,
 * there are no overruns at all.  E.g. with four equal-priority tasks,
 * if one is receiving at a rate of 115200/8 chars/second, and if
 * the task switch time is 0.01 seconds, you need to bridge three
 * tasks (i.e, 0.03 seconds) of characters, and thats 1152/8 * 3 = 432,
 * which is quite a bit.  Even with flow control, you need this buffer
 * size to transceive at maximum speed.
 *
 * Note: the buffer size can only be changed at compile time.  The
 * other settings are defaults and can be changed at run-time (see
 * UART class).
 */
#ifndef HW_CONFIG_UART_BUFFER_SIZE
#define HW_CONFIG_UART_BUFFER_SIZE        512
#endif

/**
 * Baud setting of the hardware UART.  Note that the LEON2 has no
 * hardware buffers, but can only receive and send a single character
 * before an interrupt is generated.  This means that high baud rates
 * make the CPU slow. */
#ifndef HW_CONFIG_UART_BAUD
#define HW_CONFIG_UART_BAUD               115200
#endif

/**
 * The LEON2 hardware can generate a parity bit, either even or odd.
 * See the UARTParity for possible enum values.*/
#ifndef HW_CONFIG_UART_PARITY
#define HW_CONFIG_UART_PARITY             UART_PARITY_ODD
#endif

/**
 * If the UART send by blocking (busy waiting)
 */
#ifndef HW_CONFIG_UART_BLOCKING_SEND
#define HW_CONFIG_UART_BLOCKING_SEND	  1
#endif

#ifndef HW_CONFIG_UART_FLOW_CONTROL
#define HW_CONFIG_UART_FLOW_CONTROL       0  ///< bool: use CTSN and RTSN signals
#endif

#ifndef HW_CONFIG_UART_EXTERNAL_CLOCK_HZ
#define HW_CONFIG_UART_EXTERNAL_CLOCK_HZ  0  ///< float: >0 => use external clock at this frequency
#endif

#ifndef HW_CONFIG_UART_LOOP_BACK
#define HW_CONFIG_UART_LOOP_BACK          0  ///< bool: connect input and output internally
#endif

#ifndef HW_CONFIG_UART_PANIC_ON_ERROR
#define HW_CONFIG_UART_PANIC_ON_ERROR     1  ///< cause a thread panic if there's a comm error
#endif
/**@}*/


/**
 * @defgroup hwConfigLowlevel Configuration: More stuff you'd better not change!
 *
 * Some of this is necessary for the hardware to work properly,
 * some other stuff is derives so that the settings are consistent.
 *
 * Do not change!
 *
 * Some things you *might* want to change if you really know what you
 * are doing: if you have SDRAM, you can disable SRAM.  Without RAM,
 * however, the machine won't boot, of course.
 */
/**@{*/
#ifndef HW_CONFIG_IO_ENABLE
#define HW_CONFIG_IO_ENABLE               1                            /**< boolean */
#endif

#ifndef HW_CONFIG_SRAM_ENABLE
#define HW_CONFIG_SRAM_ENABLE             (HW_CONFIG_SRAM_SIZE > 0)    /**< boolean */
#endif

#ifndef HW_CONFIG_PROM_WRITE_ENABLE
#define HW_CONFIG_PROM_WRITE_ENABLE       0                            /**< boolean */
#endif

#ifndef HW_CONFIG_SDRAM_ENABLE
#define HW_CONFIG_SDRAM_ENABLE            (HW_CONFIG_SDRAM_SIZE > 0)   /**< boolean */
#endif

#ifndef HW_CONFIG_SDRAM_REFRESH_ENABLE
#define HW_CONFIG_SDRAM_REFRESH_ENABLE    (HW_CONFIG_SDRAM_SIZE > 0)   /**< boolean */
#endif

/** in system clock ticks */
#ifndef HW_CONFIG_SDRAM_T_RP
#define HW_CONFIG_SDRAM_T_RP              ((int)(HW_DIV_CEIL( \
                                            HW_CONFIG_SYS_CLOCK_HZ, \
                                            HW_CONFIG_SDRAM_T_RP_HZ \
                                          )))
#endif

/** in system clock ticks */
#ifndef HW_CONFIG_SDRAM_T_RFC
#define HW_CONFIG_SDRAM_T_RFC             ((int)(HW_DIV_CEIL(  \
                                            HW_CONFIG_SYS_CLOCK_HZ,  \
                                            HW_CONFIG_SDRAM_T_RFC_HZ \
                                          )))
#endif


/** in system clock ticks */
#ifndef HW_CONFIG_SDRAM_REFRESH
#define HW_CONFIG_SDRAM_REFRESH           ((int)(HW_DIV_FLOOR(          \
                                            HW_CONFIG_SYS_CLOCK_HZ,    \
                                            HW_CONFIG_SDRAM_REFRESH_HZ \
                                          )))
#endif

#ifndef HW_CONFIG_SDRAM_COMMAND
#define HW_CONFIG_SDRAM_COMMAND           ((HW_CONFIG_SDRAM_SIZE > 0) ?   \
                                              0x3 /* load-command-reg */  \
                                          :   0x0 /* nothing */           \
                                          )
#endif


#ifndef HW_CONFIG_SRAM_START
#define HW_CONFIG_SRAM_START              (HW_CONFIG_SRAM_ENABLE ? 0x40000000 : 0)
#endif

#ifndef HW_CONFIG_SDRAM_START
#define HW_CONFIG_SDRAM_START             (HW_CONFIG_SDRAM_ENABLE ? \
                                            (HW_CONFIG_SRAM_ENABLE ? 0x60000000 : 0x40000000) : 0)
#endif

#ifndef HW_CONFIG_SDRAM_REFRESH_COUNTER
#define HW_CONFIG_SDRAM_REFRESH_COUNTER   (HW_CONFIG_SDRAM_ENABLE ? HW_CONFIG_SDRAM_REFRESH : 0)
#endif

#ifndef HW_CONFIG_TIME_GRANULARITY
#define HW_CONFIG_TIME_GRANULARITY        HW_CROP(                          \
                                            HW_PRESCALER_MIN + 1,           \
                                            HW_PRESCALER_MAX + 1,           \
                                            HW_DIV_FLOOR(                   \
                                              HW_CONFIG_SYS_CLOCK_HZ,       \
                                              HW_CONFIG_TIME_GRANULARITY_HZ \
                                            )                               \
                                          )
#endif

#ifndef HW_CONFIG_TIMER_CLOCK_HZ
#define HW_CONFIG_TIMER_CLOCK_HZ          HW_DIV_FLOOR(                \
                                            HW_CONFIG_SYS_CLOCK_HZ,    \
                                            HW_CONFIG_TIME_GRANULARITY \
                                          )
#endif

/** In timer clock ticks (sys clock / prescaler) */
#ifndef HW_CONFIG_WATCHDOG_TIMEOUT
#define HW_CONFIG_WATCHDOG_TIMEOUT        HW_DIV_CEIL(                    \
                                            HW_CONFIG_TIMER_CLOCK_HZ,     \
                                            HW_CONFIG_WATCHDOG_TIMEOUT_HZ \
                                          )
#endif

/** In timer clock ticks (sys clock / prescaler) */
#ifndef HW_CONFIG_WATCHDOG_BOOT_TIMEOUT
#define HW_CONFIG_WATCHDOG_BOOT_TIMEOUT   HW_DIV_CEIL(                         \
                                            HW_CONFIG_TIMER_CLOCK_HZ,          \
                                            HW_CONFIG_WATCHDOG_BOOT_TIMEOUT_HZ \
                                          )
#endif

#define HW_CONFIG_UART_BUFFER_ALLOC  (1+(HW_NEXT_POWER2MINUS1((HW_CONFIG_UART_BUFFER_SIZE)-1) | 1))

#define HW_CONFIG_SRAM_ENABLE_EDAC   (HW_CONFIG_ENABLE_EDAC && HW_CONFIG_SRAM_HAVE_EDAC)

/**@}*/

#endif /* RODOS_HW_LEON2_CORE_H */

