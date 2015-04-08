/*
 *
 */
#ifndef __UART_H__
#define __UART_H__

/***** USART for console interface ******
 *
 * UART2 is used to console interface
 * Bard rate = 115200
 * No parity
 * Word size of 8 bits
 * 1-Stop bit
 * No flow control
 */

#if defined(__cplusplus)
extern "C" {
#endif

void UART_init(int instance);
void UART_writeChar(int instance, char c);
void UART_writeStr(int instance, const char *c);
char UART_readChar(int instance);

#if defined(__cplusplus)
}
#endif

#endif //__UART_H__
