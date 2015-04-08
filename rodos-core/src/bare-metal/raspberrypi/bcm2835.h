/*
 * bcm2835.h
 *
 *  Created on: Apr 22, 2013
 *      Author: johannes freitag
 */

#include "stdint.h"

#ifndef BCM2835_H_
#define BCM2835_H_

extern "C" {
//write 32 bit to memory
extern void PUT32(uint32_t, uint32_t);
//get 32 bit from memory
extern uint32_t GET32(uint32_t);
//dummy for wait actions
extern void dummy(uint32_t);
//enable global interrupts
extern void enable_irq(void);
//disable global interrupts
extern void disable_irq(void);
//timer 1 interrupt handler
extern void TIM1_IRQHandler();
//read cpsr
extern uint32_t GETcpsr(void);
//read lr
extern uint32_t GETlr(void);
//activate mmu
extern void start_mmu(uint32_t, uint32_t);
//activate l1 cache
extern void start_l1cache();
} // end extern "C"

/***********************************************************************
 * Definitions from document Broadcom BCM2835 ARM Peripherals
 **********************************************************************/

//GPIO Function selection
#define GPFSEL0     0x20200000
#define GPFSEL1 0x20200004
#define GPSET0  0x2020001C
#define GPCLR0  0x20200028

#define GPPUD       0x20200094
#define GPPUDCLK0   0x20200098

//IO definitions
#define AUX_ENABLES     0x20215004
#define AUX_MU_IO_REG   0x20215040
#define AUX_MU_IER_REG  0x20215044
#define AUX_MU_IIR_REG  0x20215048
#define AUX_MU_LCR_REG  0x2021504C
#define AUX_MU_MCR_REG  0x20215050
#define AUX_MU_LSR_REG  0x20215054
#define AUX_MU_MSR_REG  0x20215058
#define AUX_MU_SCRATCH  0x2021505C
#define AUX_MU_CNTL_REG 0x20215060
#define AUX_MU_STAT_REG 0x20215064
#define AUX_MU_BAUD_REG 0x20215068

//Timer definitions
#define ARMBASE 0x8000
#define CS 0x20003000
#define CLOL 0x20003004
#define CLOH 0x20003008
#define C0 0x2000300C
#define C1 0x20003010
#define C2 0x20003014
#define C3 0x20003018
#define COUNTER0 0x0
#define COUNTER1 0x1
#define COUNTER2 0x2
#define COUNTER3 0x3

//Interrupt definitions
#define IRQ_BASIC 0x2000B200
#define IRQ_PEND1 0x2000B204
#define IRQ_PEND2 0x2000B208
#define IRQ_FIQ_CONTROL 0x2000B210
#define IRQ_ENABLE_BASIC 0x2000B218
#define IRQ_DISABLE_BASIC 0x2000B224
#define IRQ_DISABLE1      0x2000B21C
#define IRQ_DISABLE2      0x2000B220
#define IRQ_ENABLE1       0x2000B210
#define IRQ_ENABLE2       0x2000B214
// *****************************************************************************
// UART
// *****************************************************************************


#define AUX_MU_IIR_REG  0x20215048
#define AUX_MU_IIR_RX_IRQ ((GET32(AUX_MU_IIR_REG) & 0x07) == 0x04)
#define AUX_MU_IIR_TX_IRQ     ((GET32(AUX_MU_IIR_REG) & 0x07) == 0x02)
#define AUX_MU_LSR_REG  0x20215054
#define AUX_MU_LSR_RX_RDY     (GET32(AUX_MU_LSR_REG) & (1<<0))
#define AUX_MU_IO_REG   0x20215040
#define IRQ_PEND1         0x2000B204


// *****************************************************************************
// Broadcom Serial Controllers (BSC/I2C)
// *****************************************************************************

#define BIT(n) (1 << (n))

/* Only BSC0 is accessible from the RPi pi header.*/
#define BSC0_ADDR 0x20205000
#define BSC1_ADDR 0x20804000
#define BSC2_ADDR 0x20805000

#define BSC_CLOCK_FREQ 150000000


#define BSC_REG_C 0x0
#define BSC_REG_S 0x4
#define BSC_REG_DLEN 0x8
#define BSC_REG_A 0xc
#define BSC_REG_FIFO 0x10
#define BSC_REG_DIV 0x14
#define BSC_REG_DEL 0x18
#define BSC_REG_CLKT 0x1c

/* I2C control flags */
#define BSC_I2CEN BIT(15)
#define BSC_INTR BIT(10)
#define BSC_INTT BIT(9)
#define BSC_INTD BIT(8)
#define BSC_ST BIT(7)
#define BSC_CLEAR BIT(4)
#define BSC_READ BIT(0)

/* I2C status flags */
#define BSC_TA BIT(0) /** @brief Transfer active.*/
#define BSC_DONE BIT(1) /** @brief Transfer done.*/
#define BSC_TXW BIT(2) /** @brief FIFO needs writing.*/
#define BSC_RXR BIT(3) /** @brief FIFO needs reading.*/
#define BSC_TXD BIT(4) /** @brief FIFO can accept data.*/
#define BSC_RXD BIT(5) /** @brief FIFO contains data.*/
#define BSC_TXE BIT(6) /** @brief FIFO empty.*/
#define BSC_RXF BIT(7) /** @brief FIFO full.*/
#define BSC_ERR BIT(8) /** @brief ACK error.*/
#define BSC_CLKT BIT(9) /** @brief Clock stretch timeout.*/

/* Rising/Falling Edge Delay Defaults.*/
#define BSC_DEFAULT_FEDL 0x30
#define BSC_DEFAULT_REDL 0x30

/* Clock Stretch Timeout Defaults.*/
#define BSC_DEFAULT_CLKT 0x40

#define CLEAR_STATUS BSC_CLKT|BSC_ERR|BSC_DONE

#define START_READ BSC_I2CEN|BSC_ST|BSC_CLEAR|BSC_READ
#define START_WRITE BSC_I2CEN|BSC_ST


/*
 * MMU definitions
 */

#define MMUTABLEBASE 0x0FF000000

#define MMUTABLESIZE (0x001000000)
#define MMUTABLEMASK ((MMUTABLESIZE-1)>>2)

#define TOP_LEVEL_WORDS (1<<((31-20)+1))
#define COARSE_TABLE_WORDS (1<<((19-12)+1))
#define SMALL_TABLE_WORDS (1<<((11-0)+1))



#endif /* BCM2835_H_ */
