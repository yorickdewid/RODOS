/**
 *
 * \ingroup system_services
 * \file stdio_serial.c
 * \version 0.2
 * \date 28.08.2013
 * \author rait_ge
 *
 * \brief UART for stdoutput for Debugging
 *
 * History:
 *
 * 0.1 (rait_ge) initial version
 *
 * 0.2 (mw) add interrupt based transfer using non blocking putchar with fifos
 *
 */


#include <hw_config.h>
#include "hw_interrupt.h"
#include "string_pico.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

#ifdef UART_INTERRUPT
struct Ringbuffer {
	unsigned int pIn, pOut;
	unsigned int len;
	char data[1024];
	void* criticalArg;
};

Ringbuffer uartTxBuffer;
Ringbuffer uartRxBuffer;

bool RBput(Ringbuffer* buf, char* c) {
	bool ret = true;
	RODOS::system_enter_critical_region(buf->criticalArg);
	if (buf->len < sizeof(buf->data)){
		buf->len++;
		if (buf->pIn >= sizeof(buf->data)) buf->pIn = 0;
		buf->data[buf->pIn++] = *c;
	} else {
		ret = false;
	}
	RODOS::system_exit_critical_region(buf->criticalArg);
	return ret;
}

bool RBget(Ringbuffer* buf, char* c) {
	bool ret;
	RODOS::system_enter_critical_region(buf->criticalArg);
	ret = buf->len > 0;
	if (ret) {
		*c = buf->data[buf->pOut++];
		if (buf->pOut >= sizeof(buf->data)) buf->pOut = 0;
		buf->len--;
	}
	RODOS::system_exit_critical_region(buf->criticalArg);
	return ret;
}

void UARTTxIsr() {
	volatile u16 t = *pUART_IIR;
	ssync();
	char c;
	if ((t & 0x2) && RBget(&uartTxBuffer, &c)) {
		*pUART_THR = c;
	}
}
void UARTRxIsr() {
	volatile u16 t = *pUART_IIR;
	ssync();
	if (t & 0x4) {
		// receive data interrupt
		t = *pUART_RBR;
		ssync();
		RBput(&uartRxBuffer, (char*) t);
	} else if (t & 0x6) {
		// line status interrupt
		t = *pUART_LSR;
		ssync();
	}
}
#endif
/**
 *  \brief
 *  initialize uart driver
 *
 *  \param divisor BAUD RATE = SCLK/(16 x Divisor)
 */
int UART_inititialize(int divisor)
{
#ifdef UART_INTERRUPT
	// init internal structures
	memset((void*)&uartTxBuffer, 0, sizeof(Ringbuffer));
	memset((void*)&uartRxBuffer, 0, sizeof(Ringbuffer));
#endif
	// enable UART clock.
	*pUART_GCTL = UCEN;

	// Write result to the two 8-bit DL registers (DLH:DLL).
	*pUART_LCR = DLAB;
	*pUART_DLL = divisor;
	*pUART_DLH = divisor>>8;

	// Clear DLAB again and set UART frame to 8 bits, no parity, 1 stop bit.
	*pUART_LCR = WLS(8);
#ifdef UART_INTERRUPT
	// hook ISR
	RODOS::HW_setInterruptHandler(PERIPHERAL_INTERRUPT_CHANNEL(28), &UARTRxIsr);
	RODOS::HW_setInterruptHandler(PERIPHERAL_INTERRUPT_CHANNEL(29), &UARTTxIsr);
	// enable interrupts
	*pUART_IER = ERBFI | ETBEI;
#endif
	return 0;
}

extern "C" {
int putchar(int c)
{
#ifdef UART_INTERRUPT
	int i = 0;
	if ((uartTxBuffer.len == 0) && (*pUART_LSR & THRE)) {
		// uart idle, write directly to the hardware
		*pUART_THR = c;
	} else {
		// uart busy, write to buffer
		if (RBput(&uartTxBuffer, (char*) &c)) i = (int)c;
		else i = -1;
	}
	return i;
#else
	while (!(*pUART_LSR & THRE)) { }; //wait
	*pUART_THR = c;
	return 0;
#endif

}
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
