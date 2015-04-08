/*
 * Copyright (c) 2011 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Michael Ruffer
 */
#include <new>
#include "hal/hal_uart.h"

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class HW_HAL_UART{
public:
    UART_IDX idx;
    uint16_t hwFlowCtrl;//(USART_HardwareFlowControl_None)
    uint32_t baudrate;

    HW_HAL_UART(){
        // don't set member variables here
        // -> this constructor is only necessary to create UART_contextArray
    }

    HW_HAL_UART(UART_IDX idx){
        this->idx = idx;
        this->hwFlowCtrl = USART_HardwareFlowControl_None;
        this->baudrate = 115200;
    }
};


int uart1_init(unsigned int baudrate);
int uart2_init(unsigned int baudrate);
int uart3_init(unsigned int baudrate);
int uart4_init(unsigned int baudrate);
int uart5_init(unsigned int baudrate);

USART_TypeDef* uart_getRegPointer(int id);
int uart_rxErrorStatus(int id);

/* Multisensorboard:
 * UART1 - RS232 output (debug connector)
 * UART2 - TTL output (bluetooth module -> hardware flow control necessary)
 * UART3 - not available -> I2C_2 -> used for ITG3200 gyro
 * UART4 - TTL output (GPS module connector)
 * UART5 - not available -> USB-power-sense, USB-LinePullup-switch
 */

/* Olimex STM32-P103:
 * UART1 - TTL output (connector "UEXT" Pin3(TX), Pin4(RX))
 * UART2 - RS232 output (DSUB 9)
 * UART3 -
 */

#define GPIO_UART1_TX 			GPIO_Pin_9		// PA9
#define GPIO_UART1_RX 			GPIO_Pin_10 	// PA10
#define GPIO_UART2_TX 			GPIO_Pin_2 	 	// PA2
#define GPIO_UART2_RX 			GPIO_Pin_3 		// PA3
#define GPIO_UART3_TX 			GPIO_Pin_10		// PB10
#define GPIO_UART3_RX 			GPIO_Pin_11		// PB11
/* high density devices only */
#define GPIO_UART4_TX 			GPIO_Pin_10		// PC10
#define GPIO_UART4_RX 			GPIO_Pin_11		// PC11
#define GPIO_UART5_TX 			GPIO_Pin_12		// PC12
#define GPIO_UART5_RX 			GPIO_Pin_2		// PD2

static char uart_buf[5][UART_BUF_SIZE];

static int uartRxError[5] = { 0, 0, 0, 0, 0 };

static char* wrPos[5] = { uart_buf[0], uart_buf[1], uart_buf[2], uart_buf[3],	uart_buf[4] };
static char* rdPos[5] = { uart_buf[0], uart_buf[1], uart_buf[2], uart_buf[3],	uart_buf[4] };


extern "C" {

void USART1_IRQHandler() {

	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE )) {
		USART_ReceiveData(USART1 );
		uartRxError[0]++;
	} else if (USART_GetITStatus(USART1, USART_IT_RXNE )) {
		*wrPos[0] = (char) (USART_ReceiveData(USART1 ) & 0xFF);
		wrPos[0]++;

		if (wrPos[0] >= &uart_buf[0][UART_BUF_SIZE - 1])
			wrPos[0] = uart_buf[0];
		if (wrPos[0] == rdPos[0]) {
			uartRxError[0]++;
		}
	}

	NVIC_ClearPendingIRQ(USART1_IRQn);
}

void USART2_IRQHandler() {

	if (USART_GetFlagStatus(USART2, USART_FLAG_ORE )) {
		USART_ReceiveData(USART2 );
		uartRxError[1]++;
	} else if (USART_GetITStatus(USART2, USART_IT_RXNE )) {
		*wrPos[1] = (char) (USART_ReceiveData(USART2 ) & 0xFF);
		wrPos[1]++;

		if (wrPos[1] >= &uart_buf[1][UART_BUF_SIZE - 1])
			wrPos[1] = uart_buf[1];

		if (wrPos[1] == rdPos[1]) {
			uartRxError[1]++;
		}
	}

	NVIC_ClearPendingIRQ(USART2_IRQn);
}

void USART3_IRQHandler() {

	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE )) {
		USART_ReceiveData(USART3 );
		uartRxError[2]++;
	} else if (USART_GetITStatus(USART3, USART_IT_RXNE )) {
		*wrPos[2] = (char) (USART_ReceiveData(USART3 ) & 0xFF);
		wrPos[2]++;

		if (wrPos[2] >= &uart_buf[2][UART_BUF_SIZE - 1])
			wrPos[2] = uart_buf[2];

		if (wrPos[2] == rdPos[2]) {
			uartRxError[2]++;
		}
	}

	NVIC_ClearPendingIRQ(USART3_IRQn);
}

#ifndef STM32F10X_MD
void USART4_IRQHandler() {

	if (USART_GetFlagStatus(UART4, USART_FLAG_ORE )) {
		USART_ReceiveData(UART4 );
		uartRxError[3]++;
	} else if (USART_GetITStatus(UART4, USART_IT_RXNE )) {
		*wrPos[3] = (char) (USART_ReceiveData(UART4 ) & 0xFF);
		wrPos[3]++;

		if (wrPos[3] >= &uart_buf[3][UART_BUF_SIZE - 1])
			wrPos[3] = uart_buf[3];

		if (wrPos[3] == rdPos[3]) {
			uartRxError[3]++;
		}
	}

	NVIC_ClearPendingIRQ(UART4_IRQn);
}

void USART5_IRQHandler() {

	if (USART_GetFlagStatus(UART5, USART_FLAG_ORE )) {
		USART_ReceiveData(UART5 );
		uartRxError[4]++;
	} else if (USART_GetITStatus(UART5, USART_IT_RXNE )) {
		*wrPos[4] = (char) (USART_ReceiveData(UART5 ) & 0xFF);
		wrPos[4]++;

		if (wrPos[4] >= &uart_buf[4][UART_BUF_SIZE - 1])
			wrPos[4] = uart_buf[4];

		if (wrPos[4] == rdPos[4]) {
			uartRxError[4]++;
		}
	}

	NVIC_ClearPendingIRQ(UART5_IRQn);
}
#endif

} // end extern "C"

HW_HAL_UART UART_contextArray[5];

HAL_UART::HAL_UART(UART_IDX idx){
    if (idx < UART_IDX1) idx = UART_IDX1;
    if (idx > UART_IDX5) idx = UART_IDX5;
    context = new (&UART_contextArray[idx-1]) HW_HAL_UART(idx);
}

/*
 * USART
 * - all USART will be initialized in 8N1 mode
 */
int HAL_UART::init(unsigned int iBaudrate) {

	context->baudrate = iBaudrate;

	switch (context->idx) {
	case UART_IDX1:
		return uart1_init(iBaudrate);
	case UART_IDX2:
		return uart2_init(iBaudrate);
	case UART_IDX3:
		return uart3_init(iBaudrate);
#ifndef STM32F10X_MD
	case UART_IDX4:
		return uart4_init(iBaudrate);
	case UART_IDX5:
		return uart5_init(iBaudrate);
#endif
	default:
		return -1;
	}
}

int uart1_init(unsigned int baudrate) {

	GPIO_InitTypeDef Gis;
	USART_InitTypeDef Uis;

	// Release reset and enable clock
	USART_DeInit(USART1 );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	// Enable GPIO clock and release reset
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, DISABLE);

	// USART1 Tx (PA9) alternate output push-pull
	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_UART1_TX;
	Gis.GPIO_Mode = GPIO_Mode_AF_PP;
	Gis.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &Gis);

	// USART1 Rx (PA10) input floating
	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_UART1_RX;
	Gis.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &Gis);

	// USART1 (8N1)
	USART_StructInit(&Uis);
	Uis.USART_BaudRate = baudrate;
	USART_Init(USART1, &Uis);
	USART_Cmd(USART1, ENABLE);

	// Interrupt USART1 enable
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART1_IRQn);

	return 0;
}

int uart2_init(unsigned int baudrate) {

	GPIO_InitTypeDef Gis;
	USART_InitTypeDef Uis;

	// Release reset and enable clock
	USART_DeInit(USART2 );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	// Enable GPIO clock and release reset
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, DISABLE);

	// USART2 Tx@PA2, RTS@PA1 : alternate output push-pull
	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_UART2_TX;
	Gis.GPIO_Mode = GPIO_Mode_AF_PP;
	Gis.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &Gis);

	// USART2 Rx@PA3, CTS@PA0 : input floating
	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_UART2_RX;
	Gis.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &Gis);

	// USART2 (8N1)
	USART_StructInit(&Uis);
	Uis.USART_BaudRate = baudrate;
	USART_Init(USART2, &Uis);
	USART_Cmd(USART2, ENABLE);

	// Interrupt USART2 enable
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART2_IRQn);

	return 0;
}

int uart3_init(unsigned int baudrate) {

	GPIO_InitTypeDef Gis;
	USART_InitTypeDef Uis;

	// Release reset and enable clock
	USART_DeInit(USART3 );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	// Enable GPIO clock and release reset
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, DISABLE);

	// USART3 Tx (PB10) alternate output push-pull
	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_UART3_TX;
	Gis.GPIO_Mode = GPIO_Mode_AF_PP;
	Gis.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &Gis);

	// USART3 Rx (PB11) input floating
	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_UART3_RX;
	Gis.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &Gis);

	// USART3 (8N1)
	USART_StructInit(&Uis);
	Uis.USART_BaudRate = baudrate;
	USART_Init(USART3, &Uis);
	USART_Cmd(USART3, ENABLE);

	// Interrupt USART3 enable
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART3_IRQn);

	return 0;
}

#ifndef STM32F10X_MD
int uart4_init(unsigned int baudrate) {

	GPIO_InitTypeDef Gis;
	USART_InitTypeDef Uis;

	// Release reset and enable clock
	USART_DeInit(UART4 );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	// Enable GPIO clock and release reset
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);

	// UART4 Tx (PC10) alternate output push-pull
	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_UART4_TX;
	Gis.GPIO_Mode = GPIO_Mode_AF_PP;
	Gis.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &Gis);

	// UART4 Rx (PC11) input floating
	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_UART4_RX;
	Gis.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &Gis);

	// UART4 (8N1)
	USART_StructInit(&Uis);
	Uis.USART_BaudRate = baudrate;
	USART_Init(UART4, &Uis);
	USART_Cmd(UART4, ENABLE);

	// Interrupt UART4 enable
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(UART4_IRQn);

	return 0;
}

int uart5_init(unsigned int baudrate) {

	GPIO_InitTypeDef Gis;
	USART_InitTypeDef Uis;

	// Release reset and enable clock
	USART_DeInit(UART5 );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

	// Enable GPIO clock and release reset
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOD, DISABLE);

	// UART5 Tx (PC12) alternate output push-pull
	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_UART5_TX;
	Gis.GPIO_Mode = GPIO_Mode_AF_PP;
	Gis.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &Gis);

	// UART5 Rx (PD2) input floating
	GPIO_StructInit(&Gis);
	Gis.GPIO_Pin = GPIO_UART5_RX;
	Gis.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &Gis);

	// UART5 (8N1)
	USART_StructInit(&Uis);
	Uis.USART_BaudRate = baudrate;
	USART_Init(UART5, &Uis);
	USART_Cmd(UART5, ENABLE);

	// Interrupt UART5 enable
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(UART5_IRQn);

	return 0;
}
#endif

int HAL_UART::config(UART_PARAMETER_TYPE type, int paramVal) {

#ifndef STM32F10X_MD
	if ((context->idx < UART_IDX1) || (context->idx > UART_IDX5)) {
		return -1;
	}
#else
	if ((context->idx < UART_IDX1) || (context->idx > UART_IDX3)) {return -1;}
#endif

	USART_TypeDef* usart = uart_getRegPointer(context->idx);
	USART_InitTypeDef Uis;
	GPIO_InitTypeDef gpioRTS;
	GPIO_InitTypeDef gpioCTS;

	USART_StructInit(&Uis);
	Uis.USART_BaudRate = context->baudrate;
	Uis.USART_HardwareFlowControl = context->hwFlowCtrl;

	switch (type) {
	case UART_PARAMETER_BAUDRATE:
		if (paramVal > 0) {
			Uis.USART_BaudRate = paramVal;
			context->baudrate = paramVal;
		} else {
			return -1;
		}
		break;

	case UART_PARAMETER_HW_FLOW_CONTROL:
		if (context->idx > UART_IDX3) {
			return -1;
		} // HW flow control is only supported by USART1..3

		GPIO_StructInit(&gpioRTS);
		GPIO_StructInit(&gpioCTS);

		if (paramVal > 0) { // enable HW Flow Control
			Uis.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
			context->hwFlowCtrl = USART_HardwareFlowControl_RTS_CTS;
			gpioRTS.GPIO_Mode = GPIO_Mode_AF_PP;
			gpioRTS.GPIO_Speed = GPIO_Speed_50MHz;
			gpioCTS.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		} else {  // disable HW Flow Control
			Uis.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
			context->hwFlowCtrl = USART_HardwareFlowControl_None;
			gpioRTS.GPIO_Mode = GPIO_Mode_IN_FLOATING; // set gpio to default state
			gpioCTS.GPIO_Mode = GPIO_Mode_IN_FLOATING; // set gpio to default state
		}

		switch (context->idx) {
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
		default: // UART4/5 does not support HW flow control
		    break;
		}
		break; // end case UART_PARAMETER_HW_FLOW_CONTROL

	default:
		return -1;
	}

	USART_Cmd(usart, DISABLE);
	USART_Init(usart, &Uis);
	USART_Cmd(usart, ENABLE);

	return 0;
}

int HAL_UART::read(char* buf, int size) {
    UART_IDX idx = context->idx;

	int readCnt = 0;
	int i = 0;

#ifndef STM32F10X_MD
	if ((idx < UART_IDX1) || (idx > UART_IDX5)) {
		return -1;
	}
#else
	if ((idx < UART_IDX1) || (idx > UART_IDX3)) {return -1;}
#endif

	readCnt = this->status(UART_STATUS_RX_BUF_LEVEL);

	if (readCnt > 0) {
		if (readCnt > size) {
			readCnt = size;
		}

		for (i = 0; i < readCnt; i++) {
			*buf = *rdPos[idx - 1];
			buf++;
			rdPos[idx - 1]++;
			if (rdPos[idx - 1] >= &uart_buf[idx - 1][UART_BUF_SIZE - 1]) {
				rdPos[idx - 1] = uart_buf[idx - 1];
			}
		}

		return readCnt;
	} else {
		return -1;
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

//int HW_HAL_UART::writeStr(const char* s) {
//
//	int i = 0;
//
//	while (s[i] != 0) {
//		context->putchar(s[i]);
//		i++;
//	}
//
//	return i;
//}
//
//int HW_HAL_UART::getchar() {
//
//	char c = 0;
//
//	if (this->status(UART_STATUS_RX_BUF_LEVEL) > 0) {
//		read(&c, 1);
//		return (int) c;
//	} else {
//		return -1;
//	}
//}


bool HAL_UART::isWriteFinished() {
        UART_IDX idx = context->idx;

#ifndef STM32F10X_MD
    if ((idx < UART_IDX1) || (idx > UART_IDX5)) {
        return -1;
    }
#else
    if ((idx < UART_IDX1) || (idx > UART_IDX3)) {return -1;}
#endif

        USART_TypeDef *usart = uart_getRegPointer(idx);

        if ( (usart->SR & USART_FLAG_TXE) != 0 ) return true;
        else return false;
}


bool HAL_UART::isDataReady() {
    if (status(UART_STATUS_RX_BUF_LEVEL)>0) return true;
    else return false;
}


int HAL_UART::putcharNoWait(char c) {

        UART_IDX idx = context->idx;

#ifndef STM32F10X_MD
    if ((idx < UART_IDX1) || (idx > UART_IDX5)) {
        return -1;
    }
#else
    if ((idx < UART_IDX1) || (idx > UART_IDX3)) {return -1;}
#endif

    USART_TypeDef *usart = uart_getRegPointer(idx);

    if( isWriteFinished() == true) {
        usart->DR = c;
        return c;
    }
    else {
        return 0;
    }
}


int HAL_UART::getcharNoWait() {
    char c = 0;

    if (status(UART_STATUS_RX_BUF_LEVEL) > 0)
    {
        read(&c, 1);
        return (int)c;
    }
    else
    {
        return -1;
    }
}


int HAL_UART::status(UART_STATUS_TYPE type) {
    UART_IDX idx = context->idx;

#ifndef STM32F10X_MD
	if ((idx < UART_IDX1) || (idx > UART_IDX5)) {
		return -1;
	}
#else
	if ((idx < UART_IDX1) || (idx > UART_IDX3)) {return -1;}
#endif

	switch (type) {
	case UART_STATUS_RX_BUF_LEVEL:
		if (wrPos[idx - 1] >= rdPos[idx - 1]) {
			return wrPos[idx - 1] - rdPos[idx - 1];
		} else {
			return (wrPos[idx - 1] + UART_BUF_SIZE) - rdPos[idx - 1];
		}

	case UART_STATUS_RX_ERROR:
		return uart_rxErrorStatus(idx);
		//...
	default:
		return -1;
	}
}

int uart_rxErrorStatus(int id) {

	int overflowFlag = 0;

#ifndef STM32F10X_MD
	if ((id < 1) || (id > 5)) {
		return -1;
	}
#else
	if ((id < 1) || (id > 3)) {return -1;}
#endif

	overflowFlag = uartRxError[id - 1];

	uartRxError[id - 1] = 0;

	return overflowFlag;
}

USART_TypeDef* uart_getRegPointer(int id) {

	switch (id) {
	case 1: return USART1;
	case 2: return USART2;
	case 3: return USART3;
#ifndef STM32F10X_MD
	case 4: return UART4;
	case 5: return UART5;
#endif
	default: return USART1;
	}
}


void HAL_UART::reset(){
    UART_IDX idx = context->idx;

    if ((idx < 1) || (idx > 5)) {return;}

    USART_TypeDef *usart = uart_getRegPointer(idx);


    // Interrupt UART disable
    switch(idx){
    case 1:
    NVIC_DisableIRQ(USART1_IRQn);
        USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
        break;
    case 2:
        NVIC_DisableIRQ(USART2_IRQn);
        USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
        break;
    case 3:
        NVIC_DisableIRQ(USART3_IRQn);
        USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
        break;
#ifndef STM32F10X_MD
    case 4:
        NVIC_DisableIRQ(UART4_IRQn);
        USART_ITConfig(UART4,USART_IT_RXNE,DISABLE);
        break;
    case 5:
        NVIC_DisableIRQ(UART5_IRQn);
        USART_ITConfig(UART5,USART_IT_RXNE,DISABLE);
        break;
#endif
    default:
        break;
    }

    // disable interface and set all registers to reset state
    USART_Cmd(usart, DISABLE);
    USART_DeInit(usart);

    // reset pins
    //    - reset value of all pins: FUNCTION A, GPIO, INPUT

    GPIO_InitTypeDef Gis;
    GPIO_StructInit(&Gis);
    Gis.GPIO_Mode = GPIO_Mode_IN_FLOATING;


    switch(idx){
    case 1:
        Gis.GPIO_Pin = GPIO_UART1_RX;
        GPIO_Init(GPIOA, &Gis);
        Gis.GPIO_Pin = GPIO_UART1_TX;
        GPIO_Init(GPIOA, &Gis);
        break;
    case 2:
        Gis.GPIO_Pin = GPIO_UART2_RX;
        GPIO_Init(GPIOA, &Gis);
        Gis.GPIO_Pin = GPIO_UART2_TX;
        GPIO_Init(GPIOA, &Gis);
        break;
    case 3:
        Gis.GPIO_Pin = GPIO_UART3_RX;
        GPIO_Init(GPIOB, &Gis);
        Gis.GPIO_Pin = GPIO_UART3_TX;
        GPIO_Init(GPIOB, &Gis);
        break;
#ifndef STM32F10X_MD
    case 4:
        Gis.GPIO_Pin = GPIO_UART4_RX;
        GPIO_Init(GPIOC, &Gis);
        Gis.GPIO_Pin = GPIO_UART4_TX;
        GPIO_Init(GPIOC, &Gis);
        break;
    case 5:
        Gis.GPIO_Pin = GPIO_UART5_RX;
        GPIO_Init(GPIOD, &Gis);
        Gis.GPIO_Pin = GPIO_UART5_TX;
        GPIO_Init(GPIOC, &Gis);
        break;
#endif
    default:
        break;
    }


}


#ifndef NO_RODOS_NAMESPACE
}
#endif

