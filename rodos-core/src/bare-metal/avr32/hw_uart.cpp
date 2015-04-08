/*
 * Copyright (c) 2012, University Wuerzburg
 * Author: Michael Ruffer
 */

#include "hw_uart.h"
#include "thread.h"
#include "hw_specific.h"

//#include <string.h>



namespace RODOS {
UART::UART(const char* devname){
	if (strcmp(devname,"uart0") == 0) pHalUart = &uart0;
	if (strcmp(devname,"uart1") == 0) pHalUart = &uart1;
}

UART::~UART(){
}


//put character on UART-line
void UART::writechar(const char c){
	while (pHalUart->putcharNoWait(c) == -1){};
}

//check for incoming message
bool UART::isCharReady() {
	return pHalUart->isDataReady();
}

//read a char. from UART-RxFiFo
bool UART::getcharNoWait(char &c) {
	int tmpC = 0;
	tmpC = pHalUart->getcharNoWait();

	if (tmpC == -1) {
		return false;
	}
	else {
		c = tmpC & 0xFF;
		return true;
	}
}

//compatibility purpose to putter
bool UART::putGeneric(const unsigned int len, const void* msg) {
	pHalUart->write((const char*)msg,len);
	return true;
}
}

