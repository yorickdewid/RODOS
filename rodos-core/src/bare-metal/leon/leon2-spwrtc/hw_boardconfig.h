/* -*- Mode: C++ -*- */

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

#ifndef RODOS_HW_LEON2_SPWRTC_H
#define RODOS_HW_LEON2_SPWRTC_H

#ifndef HW_CONFIG_ARCH
#define HW_CONFIG_ARCH                    HW_ARCH_LEON2_SPWRTC
#endif

#ifndef HW_CONFIG_PROM_SIZE
#define HW_CONFIG_PROM_SIZE               (16 * 1024*1024)
#endif

#ifndef HW_CONFIG_PROM_ACCESS_HZ
#define HW_CONFIG_PROM_ACCESS_HZ          (1e9 / 115)  /**< access time in Hz = 1/s */
#endif

#ifndef HW_CONFIG_SYS_CLOCK_HZ
#define HW_CONFIG_SYS_CLOCK_HZ            (30e6)
#endif

#ifndef HW_CONFIG_HAVE_FLOAT
#define HW_CONFIG_HAVE_FLOAT              1
#endif

#ifndef HW_CONFIG_HAVE_SECOND_INT
#define HW_CONFIG_HAVE_SECOND_INT         1
#endif

#ifndef HW_CONFIG_HAVE_WATCHDOG
#define HW_CONFIG_HAVE_WATCHDOG           1
#endif

#ifndef HW_CONFIG_HAVE_EXT_WRITE_PROTECT
#define HW_CONFIG_HAVE_EXT_WRITE_PROTECT  1
#endif

#ifndef HW_CONFIG_PROM_WRITE_WAIT_STATES
#define HW_CONFIG_PROM_WRITE_WAIT_STATES  6
#endif

#ifndef HW_CONFIG_PROM_READ_WAIT_STATES
#define HW_CONFIG_PROM_READ_WAIT_STATES   6
#endif

#ifndef HW_CONFIG_SRAM_BUS_WIDTH
#define HW_CONFIG_SRAM_BUS_WIDTH          32
#endif

#ifndef HW_CONFIG_SRAM_SIZE
#define HW_CONFIG_SRAM_SIZE               (16 * 1024*1024)
#endif

#ifndef HW_CONFIG_SRAM_BANKS
#define HW_CONFIG_SRAM_BANKS              2
#endif

#ifndef HW_CONFIG_SRAM_WRITE_WAIT_STATES
#define HW_CONFIG_SRAM_WRITE_WAIT_STATES  0
#endif

#ifndef HW_CONFIG_SRAM_READ_WAIT_STATES
#define HW_CONFIG_SRAM_READ_WAIT_STATES   0
#endif

#ifndef HW_CONFIG_SRAM_HAVE_EDAC
#define HW_CONFIG_SRAM_HAVE_EDAC          1
#endif

#ifndef HW_CONFIG_SDRAM_SIZE
#define HW_CONFIG_SDRAM_SIZE              0
#endif

#ifndef HW_CONFIG_SDRAM_CAS
#define HW_CONFIG_SDRAM_CAS               2
#endif

#ifndef HW_CONFIG_SDRAM_T_RP_HZ
#define HW_CONFIG_SDRAM_T_RP_HZ           HW_DIV_FLOOR(1e9, 20.0)
#endif

#ifndef HW_CONFIG_SDRAM_T_RFC_HZ
#define HW_CONFIG_SDRAM_T_RFC_HZ          HW_DIV_FLOOR(1e9, 66.0)
#endif

#ifndef HW_CONFIG_SDRAM_BANKS
#define HW_CONFIG_SDRAM_BANKS             2
#endif

#ifndef HW_CONFIG_SDRAM_COLUMN_SIZE
#define HW_CONFIG_SDRAM_COLUMN_SIZE       4096
#endif

#ifndef HW_CONFIG_SDRAM_REFRESH_HZ
#define HW_CONFIG_SDRAM_REFRESH_HZ        HW_DIV_CEIL(1e6, 7.7)
#endif

#endif /*  RODOS_HW_LEON2_SPWRTC_H */

