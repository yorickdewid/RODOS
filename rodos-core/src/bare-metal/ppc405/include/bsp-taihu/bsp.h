/**
 * @brief BSP config: PPC405 Taihu (Qemu).
 */

#ifndef _BSP_H
#define _BSP_H

#define GPT_BASE            0xEF600000
#define UART0_BASE          0xEF600300
#define UART1_BASE          0xEF600400
#define IIC0_BASE           0xEF600500
#define OPBA_BASE           0xEF600600
#define GPIO_BASE           0xEF600700
#define ETH0_BASE           0xEF600800
#define ETH1_BASE           0xEF600900

#define UART_TX             0x00
#define UART_RX             0x00
#define UART_STATUS         0x08
#define UART_CTRL           0x0c

/* Map INTC Contoller to unused address */
#define INTC0_BASE          0xEF601000

#define INTC_IER            0x08
#define INTC_IAR            0x0c
#define INTC_IVR            0x18
#define INTC_MER            0x1c


/* Connections of Interruptcontroller */
#define INTC_UART0 2 

#endif // _BSP_H

/* vim: set noet ci pi sts=0 sw=4 ts=4: */
