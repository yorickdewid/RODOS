

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
* @file debug.cc
* @date 2008/06/17 10:48
* @author Lutz Dittrich & Sergio Montenegro,
*         adapted in 2010 by Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2008-2010 DLR
*
* @brief debug functions
*
*/

#include "rodos.h"
#include "scheduler.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

unsigned long errorCounter = 0;
//unsigned long errorCode    = 0;
const char* errorMsg    = "all OK";

Semaphore printfProtector;

void PRINTF(const char* fmt, ...) {
    if(printfVerbosity == 0) return;
    Yprintf yprintf;
    va_start(yprintf.ap, fmt);
    if (errorCounter != 0) xprintf("prev-ERR(%ld) -- ", errorCounter);
    if (!Scheduler::isSchedulerRunning()) {
        yprintf.vaprintf(fmt);
    } else {
        printfProtector.enter();
            yprintf.vaprintf(fmt);
        printfProtector.leave();
    }
    FFLUSH();
}

void PRINTF_CONDITIONAL(int negligibleLevel, const char* fmt, ...) {
    if(negligibleLevel > printfVerbosity) return;
    Yprintf yprintf;
    va_start(yprintf.ap, fmt);
    if (errorCounter != 0) xprintf("prev-ERR(%ld) -- ", errorCounter);
    if (!Scheduler::isSchedulerRunning()) {
        yprintf.vaprintf(fmt);
    } else {
        printfProtector.enter();
            yprintf.vaprintf(fmt);
        printfProtector.leave();
    }
    FFLUSH();
}

void ERROR(const char* text) {
  errorMsg = text;
  PRINTF("!! Programming-ERROR %s\n",text);
  errorCounter++;
}


//void printBin(unsigned char val) {
//	PRINTF(" ");
//	for(int i = 0; i < 8; i++) {
//		PRINTF("%c", (val & 0x80)? '1' : '0');
//		val = val << 1;
//	}
//}

void printCharsHex(const char* titel, void* ptr, unsigned int len) {
	unsigned char* vals = (unsigned char*)ptr;
	PRINTF("%s", titel);
	for(unsigned int i = 0; i < len; i++) PRINTF("%02x ", vals[i]);
	PRINTF("\n");
}

void printCharsHex_Conditional(int negligibleLevel, const char* titel, void* ptr, unsigned int len) {
        if(negligibleLevel > printfVerbosity) return;
        printCharsHex(titel,  ptr, len); 
}



char* formatBinary(long val, int len, char* outputBuf) {
	val <<= 32 - len;
	for(int i = 0; i < len; i++) {
		outputBuf[i] = ( (val & 0x80000000)? '1' : '0');
		val <<= 1;
	}
	outputBuf[len] = 0;
	return outputBuf;
}



#ifndef NO_RODOS_NAMESPACE
}
#endif


