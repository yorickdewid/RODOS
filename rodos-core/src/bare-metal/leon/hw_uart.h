/**
 * @author Andre Bartke <dev@bartke.cc>
 * 
 * @brief HAL Uart device driver interface.
 */

#ifndef RODOS_HW_LEON_UART_H
#define RODOS_HW_LEON_UART_H

#include <hal/hal_uart.h>

#define APBUART_DATA        0x00
#define APBUART_STATUS      0x04
#define APBUART_CTRL        0x08
#define APBUART_SCALER      0x0c

/* UART status register */
#define UART_STATUS_DR      0x01 /* data ready */
#define UART_STATUS_TS      0x02 /* transmitter shift register empty */
#define UART_STATUS_TH      0x04 /* transmitter hold register empty */
#define UART_STATUS_BR      0x08 /* break received */
#define UART_STATUS_OV      0x10 /* overrun */
#define UART_STATUS_PE      0x20 /* parity error */
#define UART_STATUS_FE      0x40 /* framing error */

#define UART_BUF_SIZE   256

// number of uarts present
#define UART_NUM        2

enum UARTParity {
    UART_PARITY_NONE =  0, // note that this is boolean false
    UART_PARITY_EVEN =  2, // note that this is an even value
    UART_PARITY_ODD  =  3  // note that this is an odd value
};

extern HAL_UART uart_stdout;

#endif /* RODOS_HW_LEON_UART_H */

