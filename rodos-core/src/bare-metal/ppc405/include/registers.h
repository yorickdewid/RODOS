/**
 * @file registers.h
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Control Registers.
 */

#ifndef _PPC405_REGISTERS_H
#define _PPC405_REGISTERS_H

/* MSR - Machine State Register
 *   0     13   14       16   17        19    20    21   22   23        26   27    31
 *  +----+----+----+---+----+----+---+----+-----+-----+----+-----+----+----+----+----+
 *  | -- | WE | CE | - | EE | PR | - | ME | FE0 | DWE | DE | FE1 | -- | IR | DR | -- |
 *  +----+----+----+---+----+----+---+----+-----+-----+----+-----+----+----+----+----+
 */

#define MSR_WE          (0x80000000 >> 13) /* Wait State Enable */
#define MSR_CE          (0x80000000 >> 14) /* Critical Interrupt Enable */
#define MSR_EE          (0x80000000 >> 16) /* External Interrupt Enable */
#define MSR_PR          (0x80000000 >> 17) /* Privileged Level */
#define MSR_ME          (0x80000000 >> 19) /* Machine Check Enable */
#define MSR_FE0         (0x80000000 >> 20) /* Floating-point exception mode 0 */
#define MSR_DWE         (0x80000000 >> 21) /* Debug Wait Enable */
#define MSR_DE          (0x80000000 >> 22) /* Debug interrupt Enable */
#define MSR_FE1         (0x80000000 >> 23) /* Floating-point exception mode 1 */
#define MSR_IR          (0x80000000 >> 26) /* Instruction Relocate */
#define MSR_DR          (0x80000000 >> 27) /* Data Relocate */


/* TCR - Timer Control Register
 *   0  1 2   3   4     5   6  7   8     9   10               31
 *  +----+-----+-----+-----+----+-----+-----+-------------------+
 *  | WP | WRC | WIE | PIE | FP | FIE | ARE |         --        |
 *  +----+-----+-----+-----+----+-----+-----+-------------------+
 */

#define TCR_WP          (0xc0000000 >> 0) /* Watchdog Period */
#define TCR_WP17        (0x00000000 >> 0) /* Watchdog Period 2^17 clocks */
#define TCR_WP21        (0x40000000 >> 0) /* Watchdog Period 2^21 clocks */
#define TCR_WP25        (0x80000000 >> 0) /* Watchdog Period 2^25 clocks */
#define TCR_WP29        (0xc0000000 >> 0) /* Watchdog Period 2^29 clocks */
#define TCR_WRN         (0x00000000 >> 2) /* Watchdog Reset Ctrl - No reset */
#define TCR_WRP         (0x40000000 >> 2) /* Watchdog Reset Ctrl - Processor */
#define TCR_WRC         (0x80000000 >> 2) /* Watchdog Reset Ctrl - Chip */
#define TCR_WRS         (0xc0000000 >> 2) /* Watchdog Reset Ctrl - System */
#define TCR_WIE         (0x80000000 >> 4) /* Watchdog Interrupt Enable */
#define TCR_PIE         (0x80000000 >> 5) /* PIT Interrupt Enable */
#define TCR_FIE         (0x80000000 >> 8) /* FIT Interrupt Enable */
#define TCR_ARE         (0x80000000 >> 9) /* Auto Reload Enable */


/* TSR - Timer Status Register
 *     0     1   2   3   4     5   6                          31
 *  +-----+-----+-----+-----+-----+-----------------------------+
 *  | ENW | WIS | WRS | PIS | FIS |              --             |
 *  +-----+-----+-----+-----+-----+-----------------------------+
 */
#define TSR_ENW         (0x80000000 >> 0) /* Enable Next Watchdog */
#define TSR_WIS         (0x80000000 >> 1) /* Watchdog Interrupt Status */
#define TSR_PIS         (0x80000000 >> 4) /* PIT Interrupt Status */
#define TSR_FIS         (0x80000000 >> 5) /* FIT Interrupt Status */

#endif /* _PPC405_REGISTERS_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
