/*
 * Copyright (c) 2011 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Michael Ruffer, modified by Johannes Freitag
 */
#include <hal/hal_uart.h>
#include <bcm2835.h>

#include <stdint.h>
#include <stddef.h>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

int uart0_init(uint32_t iBaudrate);

int uart_rxErrorStatus(int32_t uartIdx);

struct UART_context {
	UART_IDX idx;
	int32_t hwFlowCtrl;
	int32_t baudrate;
};

static uint8_t uart_buf[UART_BUF_SIZE];
static uint8_t* wrPos = (uint8_t*) uart_buf;
static uint8_t* rdPos = (uint8_t*) uart_buf;

class HW_HAL_UART {
	friend class ReceiveTrigger;
	friend class HAL_UART;

public:
	UART_IDX idx;
	int hwFlowCtrl;
	int baudrate;
	HAL_UART* hal_uart;

	char getBuf[UART_BUF_SIZE];
	char putBuf[UART_BUF_SIZE];

	size_t getBuf_rdPos;
	size_t getBuf_wrPos;
	size_t putBuf_rdPos;
	size_t putBuf_wrPos;

	int rxError;
} UART_contextArray[2];


HAL_UART::HAL_UART(UART_IDX uartIdx) {
	context = &UART_contextArray[uartIdx];

	context->idx = uartIdx;
	context->baudrate = 0;
	context->hwFlowCtrl = 0;
}

extern "C" {
  void UART0_IRQHandler() {
    HW_HAL_UART *context = &UART_contextArray[0];

    while (1) {
      if ((GET32(AUX_MU_IIR_REG) & 1) == 1)
        break; //no more interrupts
      if ((GET32(AUX_MU_IIR_REG) & 6) == 4) {
        //receiver holds a valid byte
        *wrPos = (uint8_t) (GET32(AUX_MU_IO_REG) & 0xFF); //read byte from rx fifo
        wrPos++;
        if (wrPos > &uart_buf[UART_BUF_SIZE - 1])
          wrPos = uart_buf;
        if (wrPos == rdPos) {
          context->rxError++;
        }
      }
    }
  }
} // end extern "C"

// initialized in 8N1 mode
int HAL_UART::init(uint32_t iBaudrate) {
	context->baudrate = iBaudrate;

	switch(context->idx) {
	case 0:
		return uart0_init(iBaudrate);
	default:
		return -1;
	}
}

int uart0_init(uint32_t baudrate) {
	PUT32(IRQ_DISABLE1, 1 << 29);
	uint32_t ra;

	//define baud, interrupts...
	PUT32(AUX_ENABLES, 1);
	PUT32(AUX_MU_IER_REG, 0);
	PUT32(AUX_MU_CNTL_REG, 0);
	PUT32(AUX_MU_LCR_REG, 3);
	PUT32(AUX_MU_MCR_REG, 0);
	PUT32(AUX_MU_IER_REG, 0x5); //enable rx interrupts
	PUT32(AUX_MU_IIR_REG, 0xC6);
	PUT32(AUX_MU_BAUD_REG, 270);

	//Map GPIO pins
	ra = GET32(GPFSEL1);
	ra &= ~(7 << 12); //gpio14
	ra |= 2 << 12;    //alt5
	ra &= ~(7 << 15); //gpio15
	ra |= 2 << 15;    //alt5
	PUT32(GPFSEL1, ra);

	//set clocks
	PUT32(GPPUD, 0);
	for (ra = 0; ra < 150; ra++)
		dummy(ra);
	PUT32(GPPUDCLK0, (1 << 14) | (1 << 15));
	for (ra = 0; ra < 150; ra++)
		dummy(ra);
	PUT32(GPPUDCLK0, 0);

	PUT32(AUX_MU_CNTL_REG, 3);

	//enable UART
	PUT32(IRQ_ENABLE1, 1 << 29);

	return 0;
}

int HAL_UART::config(UART_PARAMETER_TYPE type, int32_t paramVal) {

	return 0;

}

void HAL_UART::reset() {

}

int HAL_UART::read(char *buf, int32_t size) {
	int32_t readCnt = 0;
	int32_t i = 0;

	//read number of bytes available
	readCnt = this->status(UART_STATUS_RX_BUF_LEVEL);

	if (readCnt > 0 ) {
		if (readCnt > size) {
			readCnt = size;
		}
		//read bytes from buffer and return them
		for (i = 0; i < readCnt; i++) {
			*buf = *rdPos;
			buf++;
			rdPos++;
			if (rdPos > &uart_buf[UART_BUF_SIZE - 1]) {
				rdPos = uart_buf;
			}
		}

		return readCnt;
	}

  return 0;
}

int HAL_UART::write(const char *buf, int32_t size) {

	int32_t i = 0;
	//send them byte by byte
	for (i = 0; i < size; i++) {
		putcharNoWait(*(buf + i));
	}

	return i + 1;
}

int HAL_UART::getcharNoWait() {
	return 0;
}

int HAL_UART::putcharNoWait(char c) {
	uint32_t ca = c;
	for (int64_t i = 0; i < 100000000; ++i) {
		if (GET32(AUX_MU_LSR_REG) & 0x20)
			break;
	}

	//if the uart controller is ready send it out
	PUT32(AUX_MU_IO_REG, ca);

	return c;

}

int HAL_UART::status(UART_STATUS_TYPE type) {
	switch (type) {

	//number of bytes available
	case UART_STATUS_RX_BUF_LEVEL:
		if (wrPos >= rdPos) {
			return wrPos - rdPos;
		} else {
			return (wrPos + UART_BUF_SIZE) - rdPos;
		}

	//errors
	case UART_STATUS_RX_ERROR:
		return uart_rxErrorStatus(context->idx);

	default:
		return -1;
	}
}

bool HAL_UART::isWriteFinished() {
	return true;
}

bool HAL_UART::isDataReady() {
	return status(UART_STATUS_RX_BUF_LEVEL)>0;
}

int uart_rxErrorStatus(int32_t uartIdx) {
	HW_HAL_UART *context = &UART_contextArray[uartIdx];

	int overflowFlag = 0;
	overflowFlag = context->rxError;
	context->rxError = 0;

	return overflowFlag;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

