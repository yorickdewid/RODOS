/**
 * @file hw_uart.cpp
 * @date 2008/12/03 10:03
 * @author David Waleczek, Lutz Dittrich, Sergio Montenegro
 *         Adriano Carvalho
 *
 * Copyright 2008 DLR
 *
 * @brief UART communication
 *
 */

#include "hw_uart.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

///\todo implement uart
#warning not yet implemented

UART::UART(const char *devname) {

}

UART::~UART() {}

void UART::writechar(char c) {

}

bool UART::getcharNoWait(char &c) {
	return false;
}

bool UART::isCharReady() {
	return false;
}


bool UART::putGeneric(const unsigned int len, const void* msg) {return true;};


#ifndef NO_RODOS_NAMESPACE
}
#endif
