

/*********************************************************** Copyright 
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.
 ** 
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 ** 
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 ** 
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 ** 
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 ** 
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **  
 ****************************************************************************/


/**
 * @file hw_uart.cc
 * @date 2008/12/03 10:03
 * @author Lutz Dittrich, Sergio Montenegro
 *
 * Copyright 2008 DLR
 *
 * @brief UART communication
 *
 */

// use 'placement new'
#include <new>
#include "hw_uart.h"
#include "params.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "../api/thread.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

UART::UART(const char* devname) 
{

	int status;

    charReady=false;
    lastChar=0;

	/*
	 * Initialize the UartLite driver so that it is ready to use.
	 */
	status = XUartLite_Initialize(&UartLite, UARTLITE_DEVICE_ID);
	if (status != XST_SUCCESS) {
        exit(1);
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	status = XUartLite_SelfTest(&UartLite);
	if (status != XST_SUCCESS) {
		exit(1);
	}

	/*
	 * Initialize the send buffer bytes with a pattern to send and the
	 * the receive buffer bytes to zero.
	 */
    
    sendBuffer[0] = 0;
    recvBuffer[0] = 0;

    /*
     * Do not use interrupts.
     */
    XUartLite_DisableInterrupt(&UartLite);

}

UART::~UART() 
{
}


void UART::writechar(const char c) {

	unsigned int sentCount=0;

	/*
	 * Send the buffer through the UartLite waiting til the data can be sent
	 * (block), if the specified number of bytes was not sent successfully,
	 * then an error occurred.
	 */

    sendBuffer[0] = c;

	while (sentCount != 1) {
        sentCount = XUartLite_Send(&UartLite, sendBuffer, 1);
	}

}

bool UART::isCharReady() {

	if(charReady) return true;

	charReady = getcharNoWait(lastChar);
	return charReady;
}

bool UART::getcharNoWait(char &c) {

	unsigned int receivedCount = 0;

	if(charReady) {
		charReady = false;
		c = lastChar;
		return true;
	}

    receivedCount = XUartLite_Recv(&UartLite, recvBuffer, 1);

    if (receivedCount == 1) {
        c = recvBuffer[0];
        return true;
    }

    return false;

}

bool UART::putGeneric(const unsigned int len, const void* msg) {
	unsigned int ctr;
	for (ctr = 0; ctr<len; ctr++) {
		writechar(((const char*)msg)[ctr]);
	}
	return true;
}


void UART::getStats(XUartLite_Stats* stats) {
    XUartLite_GetStats(&UartLite, stats);
}

void UART::clearStats() {
    XUartLite_ClearStats(&UartLite);
}

void FFLUSH() {
    //	fflush(stdout);
}

/* ** Standard UART ** */

/**
 * We will use 'placement new' here to avoid usage of heap, which would
 * call xmalloc.  But if xmalloc wants to print an error message,
 * we might end up in an infinite recursion. */
static UART *getUART()
{
  static UART *x= NULL;
  static char xPlace[sizeof(UART)];
  if (x == NULL) {
      x= new ((void*)xPlace) UART ("UART0");
  }
  return x;
}

int putchar (int c)
{
    getUART()->writechar(c);

#ifdef PUTCHAR_INSERTS_CARRIAGE_RETURNS
    if (c == '\n') {
        getUART()->writechar('\r');
    }
#endif

    return 0;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

