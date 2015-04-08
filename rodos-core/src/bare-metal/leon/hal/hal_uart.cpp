/**
 * @author Andre Bartke <dev@bartke.cc>
 * 
 * @brief HAL Uart device driver interface.
 * 
 * For settings/parameters see hw_uart.h.
 */

#include <stdint.h>

#include <hal/hal_uart.h>
#include <rodos.h>

#include <hw_config.h>
#include <hw_core.h>
#include <hw_uart.h>
#include <hw_trap.h>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

// device mapping
// Note: _DATA equals base address (+offset==0)
uint32_t uartDevice[UART_NUM] = {
	(uint32_t) HW_OCR(HW_OCR_UART_1_DATA), 
	(uint32_t) HW_OCR(HW_OCR_UART_2_DATA)
};

int uart_rxErrorStatus(int32_t idx);
int putchar(int c);

class HW_HAL_UART {
	friend class ReceiveTrigger;
	friend class HAL_UART;

	// I need C-Linkage for tryToGet/tryToPut so
	// make these public for now. We may fix
	// this later on, get this running first.
public:
	UART_IDX idx;
	int hwFlowCtrl;
	int baudrate;
	HAL_UART* hal_uart;

	char getBuf[HW_CONFIG_UART_BUFFER_ALLOC];
	char putBuf[HW_CONFIG_UART_BUFFER_ALLOC];

	size_t getBuf_rdPos;
	size_t getBuf_wrPos;
	size_t putBuf_rdPos;
	size_t putBuf_wrPos;

	int rxError;
} UART_contextArray[UART_NUM];


HAL_UART::HAL_UART(UART_IDX uartIdx){
	if((uartIdx < UART_IDX0) || (uartIdx >= UART_NUM)) {
		return;
	}

	context = &UART_contextArray[uartIdx];
	context->idx = uartIdx;
	context->baudrate = 0;
	context->hwFlowCtrl = 0;
	context->hal_uart = this;
};

void tryToPut(UART_IDX uartIdx) {
	HW_HAL_UART *context = &UART_contextArray[uartIdx];
	UART_IDX idx = context->idx;

	if((idx < UART_IDX0) || (idx >= UART_NUM)) {
		return;
	}

	// base address
	volatile uint32_t data   = uartDevice[idx] + APBUART_DATA;
	// buffer positions
	size_t curRdPos = context->putBuf_rdPos;
	size_t curWrPos = context->putBuf_wrPos;

	while(curRdPos != curWrPos) {
		// Note:
		// we may run with traps disabled here, using blocking send
		// is just a bad idea which I would discourage. I still keep
		// this here if you really want to use it. -AndreB
#if HW_CONFIG_UART_BLOCKING_SEND
		// BLOCKING SEND: busy wait
		volatile uint32_t status = uartDevice[idx] + APBUART_STATUS;
		while(!(*(uint32_t*)status & UART_STATUS_TH));;
#endif
		// write to data register
		*(uint32_t*)data = (uint32_t) context->putBuf[curRdPos];
		// update fifo position
		curRdPos = (curRdPos + 1) & (HW_CONFIG_UART_BUFFER_ALLOC - 1);
	}

	// save fifo position
	context->putBuf_rdPos = curRdPos;
}

void tryToGet(UART_IDX uartIdx) {
	HW_HAL_UART *context = &UART_contextArray[uartIdx];
	UART_IDX idx = context->idx;

	if((idx < UART_IDX0) || (idx >= UART_NUM)) {
		return;
	}

	// device register
	volatile uint32_t status = uartDevice[idx] + APBUART_STATUS;
	volatile uint32_t data   = uartDevice[idx] + APBUART_DATA;

	// if there was an error, don't try again, but wait for clearance from GetChar
	if (uart_rxErrorStatus(uartIdx) == 0) {
		size_t curRdPos = context->getBuf_rdPos;
		size_t oldWrPos = context->getBuf_wrPos;
		for(;;) {
			// check for error
			if((*(uint32_t*)status) & (UART_STATUS_TH |
			                           UART_STATUS_FE |
			                           UART_STATUS_OV)) {
				context->rxError++;
			}

			// is there a character in the holding register?
			if( ((*(uint32_t*)status) & UART_STATUS_DR) == 0)
				return;

			// can we insert into the buffer or is it full?
			size_t newWrPos = (oldWrPos + 1) & (HW_CONFIG_UART_BUFFER_ALLOC - 1);
			if(curRdPos == newWrPos) {
				context->rxError++;
				return;
			}

			// read from data register and insert
			context->getBuf[oldWrPos] = *(uint32_t*)data;
			// save fifo position
			context->getBuf_wrPos = newWrPos;
			oldWrPos = newWrPos;
		}
	}
}

void uart_flush(UART_IDX uartIdx) {
	HW_HAL_UART *context = &UART_contextArray[uartIdx];
	UART_IDX idx = context->idx;

	if((idx < UART_IDX0) || (idx >= UART_NUM)) {
		return;
	}

	unsigned old= hwEnterTrapMode();
	tryToPut(uartIdx);
    int retval = context->putBuf_wrPos == context->putBuf_rdPos ? 0 : -1;
	hwLeaveTrapMode(old);

	if(retval != 0) {
		// handle error
		// try again?
		// TODO
	}
}

extern "C" {
	// UART0
	void hwTrapHandler0x12(void) {
		tryToPut(UART_IDX0);
		tryToGet(UART_IDX0);
	}
	// UART1
	void hwTrapHandler0x13(void) {
		tryToPut(UART_IDX1);
		tryToGet(UART_IDX1);
	}
	// FLUSH
	int fflush(FILE *) {
		uart_flush(UART_IDX0);
		uart_flush(UART_IDX1);
		return 0;
	}
}

// default initialization: 8N1
int HAL_UART::init(uint32_t iBaudrate) {
	switch (context->idx) {
		case 0:
			this->config(UART_PARAMETER_BAUDRATE, iBaudrate);
			this->config(UART_PARAMETER_HW_FLOW_CONTROL, HW_CONFIG_UART_FLOW_CONTROL);
			break;
		case 1:
			// uart 2 disabled
			// fallthrough
		default:
			return -1;
	}

	return 0;
}

int HAL_UART::config(UART_PARAMETER_TYPE type, int32_t paramVal) {
	switch (type) {
		case UART_PARAMETER_BAUDRATE:
			if(paramVal <= 0) {
				return -1;
			}
			// udate baudrate in uart context
			context->baudrate = paramVal;
			// update device scaler register
			// TODO: select UART_X_SCALER according to idx
			*(HW_OCR(HW_OCR_UART_1_SCALER)) =
		        		(HW_DIV_ROUND(HW_CONFIG_SYS_CLOCK_HZ, (8*paramVal))-1);
			break;
		case UART_PARAMETER_HW_FLOW_CONTROL:
			// udate flowcontrol in uart context
			context->hwFlowCtrl = paramVal;
			// yes, this is supposed to look stupid
			// I copied this part from hw_config.cpp
			// TODO: select UART_X_SCALER according to idx
			*(HW_OCR(HW_OCR_UART_1_CONTROL)) =
				(HW_BITS(HW_UART_CTRL_RE) |
		 	 	 HW_BITS(HW_UART_CTRL_TE) |
		 	 	 HW_BITS(HW_UART_CTRL_RI) |
		 	 	 HW_BITS(HW_UART_CTRL_TI) |
		 	 	 ((HW_CONFIG_UART_PARITY & 1)          ? HW_BITS(HW_UART_CTRL_PS) : 0) |
		 	 	 (HW_CONFIG_UART_PARITY                ? HW_BITS(HW_UART_CTRL_PE) : 0) |
		 	 	 (HW_CONFIG_UART_EXTERNAL_CLOCK_HZ > 0 ? HW_BITS(HW_UART_CTRL_EC) : 0) |
		 	 	 (paramVal > 0                         ? HW_BITS(HW_UART_CTRL_FL) : 0) |
		 	 	 (HW_CONFIG_UART_LOOP_BACK             ? HW_BITS(HW_UART_CTRL_LB) : 0));
			break;
			// extend for more parameter types...
		default:
			return -1;
	}

	return 0;
}

void HAL_UART::reset() {
	uint32_t old = hwEnterTrapMode();

	// reinitialize with default baudrate
	this->init();
	// reset fifo positions
	context->putBuf_wrPos = 0;
	context->putBuf_rdPos = 0;
	context->getBuf_wrPos = 0;
	context->getBuf_rdPos = 0;

	hwLeaveTrapMode(old);
}

int HAL_UART::read(char* buf, int size) {
	int32_t readCnt = 0;
	int32_t i = 0;

	// read number of bytes available
	readCnt = this->status(UART_STATUS_RX_BUF_LEVEL);

	if(readCnt > 0) {
		if(readCnt > size) {
			readCnt = size;
		}

		// read bytes from buffer and return them
		for (i = 0;i < readCnt;i++) {
			buf[i] = getcharNoWait();
		}

		return readCnt;
	}

	return 0;
}

int HAL_UART::write(const char* buf, int size) {
	int32_t i = 0;
	//send them byte by byte
	for(i = 0; i < size; i++) {
		putcharNoWait(*(buf + i));
	}

	return i + 1;
}

int HAL_UART::getcharNoWait() {
	int retval = 0;

	// Reception itself is done in the interrupt handler, so no need to
	// receive now.
	//
	// However, it may be that the reception was blocked due to overrun
	// or other error and the hold register is still holding a character.
	// This will be handled at the end of this routine.

	// Note:
	// hwLock() is not enough here, because we're concurrent with a trap handler.
	// disable interrupts and context switching completely
	uint32_t old = hwEnterTrapMode();

	uint32_t curRdPos = context->getBuf_rdPos;
	uint32_t curWrPos = context->getBuf_wrPos;

	// buffer is empty?
	if(curRdPos == curWrPos) {
		context->rxError++;
		retval = -1;
	}

	retval = (int) context->getBuf;
	context->getBuf_rdPos = (curRdPos + 1) & (HW_CONFIG_UART_BUFFER_ALLOC - 1);
	
	tryToGet(context->idx);

	hwLeaveTrapMode(old);

	return retval;
}

int HAL_UART::putcharNoWait(char c) {
	// Note:
	// hwLock() is not enough here, because we're concurrent with a trap handler.
	// disable interrupts and context switching completely
	uint32_t old = hwEnterTrapMode();

	// buffer positions
	uint32_t curRdPos = context->putBuf_rdPos;
	uint32_t curWrPos = context->putBuf_wrPos;
	uint32_t newWrPos = (curWrPos + 1) & (HW_CONFIG_UART_BUFFER_ALLOC - 1);

	if (newWrPos == curRdPos) {
		context->rxError++;
    	return 0;
	} else {
		context->putBuf[curWrPos] = c;
		context->putBuf_wrPos = newWrPos;
	}

	tryToPut(context->idx);

	hwLeaveTrapMode(old);

	return c;
}

int HAL_UART::status(UART_STATUS_TYPE type) {
	switch (type) {
		case UART_STATUS_RX_BUF_LEVEL:
			if (context->getBuf_wrPos >= context->getBuf_rdPos) {
				return context->getBuf_wrPos - context->getBuf_rdPos;
			}
			return (context->getBuf_wrPos+HW_CONFIG_UART_BUFFER_ALLOC) - context->getBuf_rdPos;
		case UART_STATUS_RX_ERROR:
			return uart_rxErrorStatus(context->idx);
		// extend if necessary ...
		default:
			return -1;
	}
}

bool HAL_UART::isWriteFinished() {
	volatile uint32_t status = uartDevice[context->idx] + APBUART_STATUS;
	if(!((*(uint32_t*)status) & UART_STATUS_TH)) {
		return false;
	}

	return true;
}

bool HAL_UART::isDataReady() {
	if(status(UART_STATUS_RX_BUF_LEVEL) > 0) {
		return true;
	}

	return false;
}

int uart_rxErrorStatus(int32_t uartIdx) {
	HW_HAL_UART *context = &UART_contextArray[uartIdx];

	int overflowFlag = 0;
	overflowFlag = context->rxError;
	context->rxError = 0;

	return overflowFlag;
}

extern "C" {
	int putchar(int ic) {
		char c = (char) (ic & 0xff);

		while(!uart_stdout.isWriteFinished());;
		uart_stdout.putcharNoWait(c);

		if(c == '\n') {
			putchar('\r');
		}

		return c;
	}
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

