/*!
 * \file    uartExample.cpp
 *
 * \date    30.10.2012
 * \author  Michael Ruffer
 *
 * \brief   Shows how to use RODOS HAL_UART. To run this example connect your PC
 *          to uart2 according hal_uart.cpp of your target
 *          e.g. STM32F4 pin PD5 & PD6
 */
#include "rodos.h"
#include "hal/hal_uart.h"

HAL_UART uart2(UART_IDX2);

class UartTest: public Thread {
	uint64_t periode;
	HAL_UART *uart;

public:
	UartTest(const char* name, HAL_UART *uart, uint64_t periode) : Thread(name) {
		this->periode = periode;
		this->uart = uart;
	}

	void init(){
	    uart->init(115200);
	}

	void run() {
		char ch[32];
		const char string[] = "character received: ";
		TIME_LOOP(0,periode){
		    int32_t retVal = uart->read(ch,32);
			if (retVal > 0){
				write(string,sizeof(string)-1);
				write(ch,retVal);
				write("\n\r",2);
			}
		}
	}

	void write(const char *buf, int size){
	    int sentBytes=0;
	    int retVal;
	    while(sentBytes < size){
	        retVal = uart->write(&buf[sentBytes],size-sentBytes);
	        if (retVal < 0){
	            PRINTF("UART sent error\n");
	        }else{
	            sentBytes+=retVal;
	        }
	    }
	}
};

UartTest u1("UartTest", &uart2, 100 * MILLISECONDS);
/***********************************************************************/
