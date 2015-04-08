/**
 * @brief BSP config: Xilinx XC4VFX60.
 */

#ifndef _BSP_H
#define _BSP_H


#ifdef USE_DEV_BOARD_BIROS
	/* Using settings for the Biros FPGA Config */
	#define UART0_BASE          0x81A00000
	#define INTC0_BASE          0x81800000

	/* Connections of Interruptcontroller */
	#define INTC_UART0 4
	#define INTC_TC2 0
	#define INTC_TC1 1

	/* --- PPU --- */
	/* DLR Version 20.01.2014 */
	/* Base Address of PPU */
	#define PPU_BASE			0x81A10000

	/* - Offsets - */
	/* PPU Components */
	#define PPU_OFF				0x00
	#define PPU_ON				0x04
	/* UART */
	#define TC_CTRL				0x08
	#define TC1_DATA			0x0c
	#define TC2_DATA			0x10
	/* SPI */
	#define SPI_CTRL			0x14
	#define SPI1_DATA			0x18
	#define SPI2_DATA			0x1c
	#define SPI3_DATA			0x20
	#define SPI4_DATA			0x1c
	/* Debug */
	#define PPU_DEBUG			0x7c

#elif USE_DEV_BOARD_OLDSSMMU
    // Using settings for the old config
	#define UART0_BASE          0x84000000
	#define INTC0_BASE          0x81800000

	/* Connections of Interruptcontroller */
	#define INTC_UART0 2

#else
	#error No Development Board specified!
#endif

/* General Offsets */

/* UART */
#define UART_RX             0x00
#define UART_TX             0x04
#define UART_STATUS         0x08
#define UART_CTRL           0x0c

/* Interruptcontroller */
#define INTC_IER			0x08
#define INTC_IAR			0x0c
#define INTC_IVR			0x18
#define INTC_MER			0x1c


#endif // _BSP_H

/* vim: set noet ci pi sts=0 sw=4 ts=4: */
