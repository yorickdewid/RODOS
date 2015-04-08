/**
 * @file addresses.h
 * @author Tobias Mikschl <tobias.mikschl@uni-wuerzburg.de>
 *
 * @brief Addresses of external IO based on the values set in bsp.h.
 */
#include <bsp.h>

#ifndef _PPC405_ADDRESSES_H
#define _PPC405_ADDRESSES_H

volatile unsigned int *const UART0_BASEADDR = (unsigned int *) UART0_BASE;
static const unsigned int uart_rxOffset     = UART_RX   / sizeof(unsigned int);
static const unsigned int uart_txOffset     = UART_TX / sizeof(unsigned int);
static const unsigned int uart_ctrlOffset   = UART_CTRL / sizeof(unsigned int);
static const unsigned int uart_statOffset   = UART_STATUS / sizeof(unsigned int);

volatile unsigned int *const INTC0_BASEADDR = (unsigned int *)INTC0_BASE;
static const unsigned int intc_ierOffset  = INTC_IER / sizeof(unsigned int);
static const unsigned int intc_iarOffset  = INTC_IAR / sizeof(unsigned int);
static const unsigned int intc_ivrOffset  = INTC_IVR / sizeof(unsigned int);
static const unsigned int intc_merOffset  = INTC_MER / sizeof(unsigned int);


#ifdef USE_DEV_BOARD_BIROS
volatile unsigned int *const PPU_BASEADDR     = (unsigned int *)PPU_BASE;
static const unsigned int ppu_offOffset       = PPU_OFF / sizeof(unsigned int);
static const unsigned int ppu_onOffset        = PPU_ON / sizeof(unsigned int);
static const unsigned int ppu_tcCtrlOffset    = TC_CTRL / sizeof(unsigned int);
static const unsigned int ppu_tc1DataOffset   = TC1_DATA / sizeof(unsigned int);
static const unsigned int ppu_tc2DataOffset   = TC2_DATA / sizeof(unsigned int);
static const unsigned int ppu_spiCtrlOffset   = SPI_CTRL / sizeof(unsigned int);
static const unsigned int ppu_spi1DataOffset  = SPI1_DATA / sizeof(unsigned int);
static const unsigned int ppu_spi2DataOffset  = SPI2_DATA / sizeof(unsigned int);
static const unsigned int ppu_spi3DataOffset  = SPI3_DATA / sizeof(unsigned int);
static const unsigned int ppu_spi4DataOffset  = SPI4_DATA / sizeof(unsigned int);
static const unsigned int ppu_debugOffset     = PPU_DEBUG / sizeof(unsigned int);
#endif

#endif /* _PPC405_ADDRESSES_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
