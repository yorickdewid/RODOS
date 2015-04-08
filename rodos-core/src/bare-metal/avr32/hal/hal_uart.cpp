/*
 * Copyright (c) 2011 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Michael Ruffer
 */

#include "hal/hal_uart.h"
#include "params.h"

extern "C"
{
#include <avr32/io.h>
#include "evk1100.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "usart.h"
#include "intc.h"
}

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif



volatile avr32_usart_t* uart_getRegPointer(int idx);
int uart_rxErrorStatus(int idx);

/* EVK1100:
   UART0 - RS232 output
   UART1 - RS232 output
   UART2 -
   UART3 -
 */


static char uart_buf[4][UART_BUF_SIZE];

static int uartRxError[4] = {0,0,0,0};

static char* wrPos[4] = {uart_buf[0],uart_buf[1],uart_buf[2],uart_buf[3]};
static char* rdPos[4] = {uart_buf[0],uart_buf[1],uart_buf[2],uart_buf[3]};


class HW_HAL_UART {
public:
	UART_IDX idx;
	//int hwFlowCtrl;
	int baudrate;
};


// provide memory for context of HAL_UART-objects 
// -> maybe someone likes to create more than one object per UART-interface
//    so we provide more space (10) than there are real UART-interfaces (4)
#define MAX_UART_OBJECTS 10
static HW_HAL_UART HW_HAL_UARTArray[MAX_UART_OBJECTS];
static int uartCtxArrIdx = 0;

HAL_UART::HAL_UART(UART_IDX uartIdx){
	context = &HW_HAL_UARTArray[uartCtxArrIdx];// static mem alloc
    // make sure that we don't exeed array
    if (uartCtxArrIdx < (MAX_UART_OBJECTS-1) ) uartCtxArrIdx++; 

	HW_HAL_UART *pContext = (HW_HAL_UART*)context;
	pContext->idx = uartIdx;
	pContext->baudrate = 115200;
};

static void uart_commonIRQHandler(volatile avr32_usart_t* usart, UART_IDX uartId){

	int c = 0;

	if ( (usart->csr & (AVR32_USART_CSR_FRAME_MASK | AVR32_USART_CSR_OVRE_MASK | AVR32_USART_CSR_PARE_MASK)) != 0)
	{
		usart_reset_status(usart);
		uartRxError[uartId]++;
	}
	else if (usart_read_char(usart, &c) == USART_SUCCESS)
	{
		*wrPos[uartId] = (char)(c & 0xFF);
		wrPos[uartId]++;

		if (wrPos[uartId] >= &uart_buf[uartId][UART_BUF_SIZE-1]) wrPos[uartId] = uart_buf[uartId];
		if (wrPos[uartId] == rdPos[uartId]) {uartRxError[uartId]++;}
	}
}


__attribute__((__interrupt__))
static void USART0_IRQHandler() {
	uart_commonIRQHandler(&AVR32_USART0,UART_IDX0);
}


__attribute__((__interrupt__))
static void USART1_IRQHandler() {
	uart_commonIRQHandler(&AVR32_USART1,UART_IDX1);
}


__attribute__((__interrupt__))
static void USART2_IRQHandler() {

	uart_commonIRQHandler(&AVR32_USART2,UART_IDX2);
}


__attribute__((__interrupt__))
static void USART3_IRQHandler() {
	uart_commonIRQHandler(&AVR32_USART3,UART_IDX3);
}


/*
 * USART
 * - all USART will be initialized in 8N1 mode
 */
int HAL_UART::init(unsigned int iBaudrate) {
	HW_HAL_UART *pContext = (HW_HAL_UART*)context;
	UART_IDX idx = pContext->idx;

	if ((idx < 0) || (idx > 3)) {return -1;}

	volatile avr32_usart_t* uart = uart_getRegPointer(idx);

	// USART options.
	const usart_options_t USART_OPTIONS =
	{
		iBaudrate,			// baudrate
		8, 					// charlength
		USART_NO_PARITY, 	// paritytype
		USART_1_STOPBIT, 	// stopbits
		USART_NORMAL_CHMODE // channelmode
	};

	gpio_map_t USART_GPIO_MAP = { {0,0}, {0,0} };


	pContext->baudrate = iBaudrate;

	switch (idx)	{
		case 0:
			USART_GPIO_MAP[0].pin 		= AVR32_USART0_RXD_0_0_PIN;
			USART_GPIO_MAP[0].function 	= AVR32_USART0_RXD_0_0_FUNCTION;
			USART_GPIO_MAP[1].pin 		= AVR32_USART0_TXD_0_0_PIN;
			USART_GPIO_MAP[1].function	= AVR32_USART0_TXD_0_0_FUNCTION;
			INTC_register_interrupt(&USART0_IRQHandler, AVR32_USART0_IRQ, AVR32_INTC_INT0);
			break;
		case 1:
			USART_GPIO_MAP[0].pin 		= AVR32_USART1_RXD_0_0_PIN;
			USART_GPIO_MAP[0].function 	= AVR32_USART1_RXD_0_0_FUNCTION;
			USART_GPIO_MAP[1].pin 		= AVR32_USART1_TXD_0_0_PIN;
			USART_GPIO_MAP[1].function	= AVR32_USART1_TXD_0_0_FUNCTION;
			INTC_register_interrupt(&USART1_IRQHandler, AVR32_USART1_IRQ, AVR32_INTC_INT0);
			break;
		case 2:
			USART_GPIO_MAP[0].pin 		= AVR32_USART2_RXD_0_0_PIN;
			USART_GPIO_MAP[0].function 	= AVR32_USART2_RXD_0_0_FUNCTION;
			USART_GPIO_MAP[1].pin 		= AVR32_USART2_TXD_0_0_PIN;
			USART_GPIO_MAP[1].function	= AVR32_USART2_TXD_0_0_FUNCTION;
			INTC_register_interrupt(&USART2_IRQHandler, AVR32_USART2_IRQ, AVR32_INTC_INT0);
			break;
		case 3:
			USART_GPIO_MAP[0].pin 		= AVR32_USART3_RXD_0_0_PIN;
			USART_GPIO_MAP[0].function 	= AVR32_USART3_RXD_0_0_FUNCTION;
			USART_GPIO_MAP[1].pin 		= AVR32_USART3_TXD_0_0_PIN;
			USART_GPIO_MAP[1].function	= AVR32_USART3_TXD_0_0_FUNCTION;
			INTC_register_interrupt(&USART3_IRQHandler, AVR32_USART3_IRQ, AVR32_INTC_INT0);
			break;
		default: return -1;
	}

	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP, sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	// Initialize USART in RS232 mode.
	usart_init_rs232(uart, &USART_OPTIONS, PBA_CLK);

	// Enable USART Rx interrupt.
	uart->ier = AVR32_USART_IER_RXRDY_MASK;

	return 0;
}


int HAL_UART::config(UART_PARAMETER_TYPE type, int paramVal) {
	HW_HAL_UART *pContext = (HW_HAL_UART*)context;
	UART_IDX idx = pContext->idx;

	if ((idx < 0) || (idx > 3)) {return -1;}

	volatile avr32_usart_t* uart = uart_getRegPointer(idx);

	usart_options_t USART_OPTIONS =
	{
		pContext->baudrate,	// set current baudrate
		8, 					// charlength
		USART_NO_PARITY, 	// paritytype
		USART_1_STOPBIT, 	// stopbits
		USART_NORMAL_CHMODE // channelmode
	};
	
	switch (type) {
		case UART_PARAMETER_BAUDRATE:
			if (paramVal > 0) 
			{
				USART_OPTIONS.baudrate = paramVal; // set new baudrate
				pContext->baudrate = paramVal;
				if (usart_init_rs232(uart, &USART_OPTIONS, PBA_CLK) == USART_SUCCESS) {
					// re-enable USART Rx interrupt.
					uart->ier = AVR32_USART_IER_RXRDY_MASK;
					return 0;
				}
			}
			return -1;
#if 0 // old stuff from stm32-driver -> must be adapted for avr32
		case UART_PARAMETER_HW_FLOW_CONTROL:
			if (idx > 3) {return -1;} // HW flow control is only supported by USART1..3

			GPIO_StructInit(&gpioRTS);
			GPIO_StructInit(&gpioCTS);

			if (paramVal > 0){ // enable HW Flow Control
				Uis.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
				this->hwFlowCtrl = USART_HardwareFlowControl_RTS_CTS;
				gpioRTS.GPIO_Mode = GPIO_Mode_AF_PP;
				gpioRTS.GPIO_Speed = GPIO_Speed_50MHz;
				gpioCTS.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			}
			else {  // disable HW Flow Control
				Uis.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
				this->hwFlowCtrl = USART_HardwareFlowControl_None;
				gpioRTS.GPIO_Mode = GPIO_Mode_IN_FLOATING; // set gpio to default state
				gpioCTS.GPIO_Mode = GPIO_Mode_IN_FLOATING; // set gpio to default state
			}

			switch (idx)	{
				case 1: // USART1: CTS PA11; RTS PA12;
					gpioCTS.GPIO_Pin = GPIO_Pin_11;
					GPIO_Init(GPIOA, &gpioCTS);
					gpioRTS.GPIO_Pin = GPIO_Pin_12;
					GPIO_Init(GPIOA, &gpioRTS);
					break;
				case 2: // USART2: CTS PA0; RTS PA1;
					gpioCTS.GPIO_Pin = GPIO_Pin_0;
					GPIO_Init(GPIOA, &gpioCTS);
					gpioRTS.GPIO_Pin = GPIO_Pin_1;
					GPIO_Init(GPIOA, &gpioRTS);
					break;
				case 3: // USART3: CTS PB13; RTS PB14;
					gpioCTS.GPIO_Pin = GPIO_Pin_13;
					GPIO_Init(GPIOB, &gpioCTS);
					gpioRTS.GPIO_Pin = GPIO_Pin_14;
					GPIO_Init(GPIOB, &gpioRTS);
					break;
			}
			break; // end case UART_PARAMETER_HW_FLOW_CONTROL
#endif
		default: return -1;
	}
}


extern "C" void _unhandled_interrupt(); // defined in avr32_framework/intc.c

void HAL_UART::reset(){
	HW_HAL_UART *pContext = (HW_HAL_UART*)context;
	UART_IDX idx = pContext->idx;

	if ((idx < 0) || (idx > 3)) {return;}

	volatile avr32_usart_t* uart = uart_getRegPointer(idx);

	int dummy = 0;

	/* disable interface and set all registers to reset state */
	// w - Control Register
	uart->cr = 	AVR32_USART_CR_RXDIS_MASK 	| 		// receiver disable
				AVR32_USART_CR_TXEN_MASK 	|		// transmitter disable
				AVR32_USART_CR_RSTRX_MASK 	|		// reset receiver
				AVR32_USART_CR_RSTTX_MASK 	|		// reset transmitter
				AVR32_USART_CR_RSTSTA_MASK 	|		// reset status bits
				AVR32_USART_CR_RSTIT_MASK 	|		// reset iterations
				AVR32_USART_CR_RSTNACK_MASK;		// reset non acknowledge

	uart->mr = 0;				// rw - Mode Register
	//uart->ier = 0;			// w - Interrupt Enable Register
	uart->idr = 0x01183fff;		// w - Interrupt Disable Register -> disable all interrupts
	dummy = uart->imr;			// r - Interrupt Mask Register
	dummy = uart->csr;			// r - Channel Status Register
	dummy = uart->rhr;			// r - Receiver Holding Register
	uart->thr = 0;				// w - Transmitter Holding Register
	uart->brgr = 0;				// rw - Baud Rate GEnerator Register
	uart->rtor = 0;				// rw - Receiver Time-Out Register
	uart->ttgr = 0;				// rw - Transmitter Timeguard Register
	uart->fidi = 0x174;			// rw - FI DI Ratio Register
	dummy = uart->ner;			// r - Number of Erros Register -> clears on read
	uart->ifr = 0;				// rw - IrDA Filter Register
	uart->man = 0x30011004;		// rw - Manchester Encoder Decoder Register


	/* reset pins
	 * - reset value of all pins: FUNCTION A, GPIO, INPUT
	 */
	gpio_map_t USART_GPIO_MAP = { {0,0}, {0,0} };

	USART_GPIO_MAP[0].function 	= 0; // FUNCTION A
	USART_GPIO_MAP[1].function	= 0; // FUNCTION A

	switch (idx)	{
		case 0:
			USART_GPIO_MAP[0].pin 		= AVR32_USART0_RXD_0_0_PIN;
			USART_GPIO_MAP[1].pin 		= AVR32_USART0_TXD_0_0_PIN;
			INTC_register_interrupt(&_unhandled_interrupt, AVR32_USART0_IRQ, AVR32_INTC_INT0);
			break;
		case 1:
			USART_GPIO_MAP[0].pin 		= AVR32_USART1_RXD_0_0_PIN;
			USART_GPIO_MAP[1].pin 		= AVR32_USART1_TXD_0_0_PIN;
			INTC_register_interrupt(&_unhandled_interrupt, AVR32_USART1_IRQ, AVR32_INTC_INT0);
			break;
		case 2:
			USART_GPIO_MAP[0].pin 		= AVR32_USART2_RXD_0_0_PIN;
			USART_GPIO_MAP[1].pin 		= AVR32_USART2_TXD_0_0_PIN;
			INTC_register_interrupt(&_unhandled_interrupt, AVR32_USART2_IRQ, AVR32_INTC_INT0);
			break;
		case 3:
			USART_GPIO_MAP[0].pin 		= AVR32_USART3_RXD_0_0_PIN;
			USART_GPIO_MAP[1].pin 		= AVR32_USART3_TXD_0_0_PIN;
			INTC_register_interrupt(&_unhandled_interrupt, AVR32_USART3_IRQ, AVR32_INTC_INT0);
			break;
		default: return;
	}

	// enable GPIO as INPUT
	gpio_enable_gpio(USART_GPIO_MAP, sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));
}


int HAL_UART::read(char* buf, int size) {
	HW_HAL_UART *pContext = (HW_HAL_UART*)context;
	UART_IDX idx = pContext->idx;

	int readCnt = 0;
	int i = 0;
	
	if ((idx < 0) || (idx > 3)) {return -1;}


	readCnt = this->status(UART_STATUS_RX_BUF_LEVEL);

	if (readCnt > 0)
	{	
		if (readCnt > size) {readCnt = size;}

		for (i = 0; i < readCnt; i++ )
		{
			*buf = *rdPos[idx];
			buf++;
			rdPos[idx]++;
			if (rdPos[idx] >= &uart_buf[idx][UART_BUF_SIZE-1])
				{rdPos[idx] = uart_buf[idx];}
		}

		return readCnt;
	}
	else
	{
		return 0;
	}
}


int HAL_UART::write(const char* buf, int size) {

	int i = 0;

	for (i=0;i<size;i++)
	{
		while(isWriteFinished() == false) {}
		putcharNoWait(*(buf+i));
	}

	return i+1;	
}

/*
int UART::writeStr(const char* s) {

	int i = 0;

	while (s[i] != 0)
	{
		putchar(s[i]);
		i++;
	}

	return i;	
}*/


int HAL_UART::getcharNoWait() {
	char c = 0;

	if (status(UART_STATUS_RX_BUF_LEVEL) > 0)
	{
		read(&c, 1);
		return (int)c;
	}
	else
	{
		return 0;
	}
}


int HAL_UART::putcharNoWait(char c) {
	HW_HAL_UART *pContext = (HW_HAL_UART*)context;
	UART_IDX idx = pContext->idx;

	if ((idx < 0) || (idx > 3)) {return -1;}

	volatile avr32_usart_t *usart = uart_getRegPointer(idx);

	if( isWriteFinished() == true) {
		usart_write_char(usart, c);
		return c;
	}
	else {
		return 0;
	}
}


int HAL_UART::status(UART_STATUS_TYPE type) {
	HW_HAL_UART *pContext = (HW_HAL_UART*)context;
	UART_IDX idx = pContext->idx;

	if ((idx < 0) || (idx > 3)) {return -1;}

	switch (type)
	{
		case UART_STATUS_RX_BUF_LEVEL:
			if (wrPos[idx] >= rdPos[idx]) {return wrPos[idx] - rdPos[idx];}
			else {return (wrPos[idx]+UART_BUF_SIZE) - rdPos[idx];}

		case UART_STATUS_RX_ERROR:
			return uart_rxErrorStatus(idx);
		//...
		default: return -1;
	}
}


bool HAL_UART::isWriteFinished() {
	HW_HAL_UART *pContext = (HW_HAL_UART*)context;
	UART_IDX idx = pContext->idx;

	if ((idx < 0) || (idx > 3)) {return false;}

	volatile avr32_usart_t *usart = uart_getRegPointer(idx);

	return usart_tx_ready(usart) != 0;
}


bool HAL_UART::isDataReady() {
	if (status(UART_STATUS_RX_BUF_LEVEL)>0)	return true;
	else return false;
}


int uart_rxErrorStatus(int idx) {
	int overflowFlag = 0;
	
	if ((idx < 0) || (idx > 3)) {return -1;}

	overflowFlag = uartRxError[idx];

	uartRxError[idx] = 0;

	return overflowFlag;
}


volatile avr32_usart_t* uart_getRegPointer(int idx) {
	switch (idx)
	{
		case 0: return (&AVR32_USART0);
		case 1: return (&AVR32_USART1);
		case 2: return (&AVR32_USART2);
		case 3: return (&AVR32_USART3);
		default: return (&AVR32_USART0);
	}
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
