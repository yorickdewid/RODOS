/*
 * Implementation of the console interface.
 * The console interface is connected, with the supplier cable,
 * to the UART2. See Hardware User Manual Revision 2 for more info!
 */

#include "console_interface.h"
#include "registers/regsuart.h"


//void console_initialize(uint32_t baudrate, uint8_t parity, uint8_t stopbits, uint8_t datasize, uint8_t flowcontrol){
//	uart_iomux_config(HW_UART2);
//	uart_init(&console_interface , baudrate, parity, stopbits, datasize, flowcontrol);
//}

char console_putChar(char c){
	while (HW_UART_UTS(2).B.TXFULL);

	HW_UART_UTXD_WR(2, c);
	if(c == '\n'){
		HW_UART_UTXD_WR(2, '\r');
	}

	return (char)c;
}

int console_putStr(char *c){
	while (*c != 0){
		console_putChar(*c);
		c++;
	}
	return 0;
}

char console_readChar(){
	unsigned int read_data;

	/* If Rx FIFO has no data ready */
	if (!(HW_UART_USR2(2).B.RDR))
		return -1;

	read_data = HW_UART_URXD_RD(2);

	/* If error are detected */
	if (read_data & 0x7C00)
		return -1;

	return (char) (read_data & 0xFF);
}

void console_flush(){
	while (console_readChar() != -1);
}
