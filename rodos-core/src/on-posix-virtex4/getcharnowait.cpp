

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



//#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "hw_uart.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/*
 * nonblocking getc / getchar (from stdin)
 */
/****************************************************/

static UART uart0 ("uart0");

/** Returns -1 if no data, else char from getchar **/
int getcharNoWait() {
    char c;

	if (uart0.getcharNoWait(c)) return (unsigned char)c;
    else return -1;
}

bool isCharReady() {
    return uart0.isCharReady();
}

char* getsNoWait() {
	static char inBuffer[122];
	static int inBufferIndex = 0;

	while(1) { // terminated with returns

		int c= getcharNoWait();
		if(c < 0) return 0;

		if(c == '\n') { // end of string -> return
			inBuffer[inBufferIndex] = 0;
			inBufferIndex = 0;
			return inBuffer;
		}

		inBuffer[inBufferIndex] = c;
		if(inBufferIndex < 120) inBufferIndex++;
	}

}

void getUARTStats(XUartLite_Stats* stats) {
    uart0.getStats(stats);
}

void clearUARTStats() {
    uart0.clearStats();
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

