/**
 * @file	hw_debug_uart.cpp
 * @brief	Simple Implementation of UART0
 * 
 * Used for debugging purposes.
 * 
 * @date	May 2011
 * @author	Fabian Greif
 */

#include "lpc24xx_registers.h"
#include "hw_debug_uart.h"
namespace RODOS {
char* getsNoWait() {
 static int index = 0;
 static  char inputS[200];
 char c;

 while (U0LSR & 0x1) {
 c = U0RBR;
 inputS[index] = c;
 if(index <= 199) index++;
 if(c == 0 || c == '\n') {
 index = 0;
 return inputS;
 }
 }
 return 0;
}

// ----------------------------------------------------------------------------
void
DebugUart::initialize()
{
	// power up
	PCONP_p->peripheral.UART0 = 1; // power up UART0

	// peripheral clock
	PCLKSEL0_p->peripheral.UART0 = 0; // pclk = CCLK/4 = 18 MHz

	// select peripheral mode
	PINSEL0 = (PINSEL0 & ~0x000000F0) | 0x00000050; // Select UART0 RXD/TXD
	PINMODE0 = (PINMODE0 & ~0x000000F0) | 0x000000A0; // neither pullup nor pulldown


	// desired baud rate  115200
	// MulVal = 9
	// DivAddVal = 2
	// UnDLL = 8
	// UnDLM = 0
	// line control, enable access to divisor latches
	U0LCR = 0x83; // 8N1, enable Divisor latch bit

	// desired baud rate  115200
	U0FDR = 0x92; // DivAddVal & MulVal
	U0DLL = 8;
	U0DLM = 0;
	// desired baud rate 9600 
	//U0FDR = 0xD3; // DivAddVal & MulVal
	//U0DLL = 0x5F;
	//U0DLM = 0;

	// disable interrupts
	U0IER = 0;
	U0FCR = 0x07; // Enable and clear FIFO's
	U0LCR = 3; // Disable Divisor latch bit
	// 8 bits chars, 1 stop bit, no parity, odd parity, no break,
}

void
DebugUart::writechar(const char c)
{
	while (!(U0LSR & 0x20)) {
		// wait until the send buffer is free
	}
	U0THR = c;
}

void
DebugUart::flush()
{
	while (!(U0LSR & 0x20)) {}
}

bool
DebugUart::isCharacterAvailable()
{
	return (U0LSR & 0x1);
}

bool
DebugUart::getcharNoWait(char &c)
{
	if (U0LSR & 0x1) {
		c = U0RBR;
		return true;
	}
	c = -1;
	return false;
}
}
