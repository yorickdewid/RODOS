

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

#include "rodos.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
//#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

namespace RODOS {

static bool initialized = false;

/* 
 * Getchar as interrupt: Signal
 */

static char inputBuf[500];
static void charReader(int signo) {
    fd_set         read_set;
    struct timeval timeout_value;

    GenericMsgRef msgRef;
    msgRef.msgPtr  = inputBuf;

    FD_ZERO(&read_set);
    FD_SET(STDIN_FILENO,&read_set);

    timeout_value.tv_sec =0;
    timeout_value.tv_usec=0;

    if(select(STDIN_FILENO+1, &read_set, NULL, NULL, &timeout_value) > 0) {
        msgRef.msgLen = read(STDIN_FILENO, inputBuf, 100);
        inputBuf[msgRef.msgLen] = 0;
        charInput.publishFromInterrupt(&msgRef, sizeof(GenericMsgRef));
    }
}


void activateTopicCharInput() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = charReader;
    sigaction(SIGIO, &sa, NULL);
    fcntl(0, F_SETOWN, getpid());
    fcntl(0, F_SETFL, O_NONBLOCK|O_ASYNC);

    // initialized = true; 
}



/*
 * nonblocking getc / getchar (from stdin)
 */

void setNoDelay() {
	struct termios term;

	//fcntl(kb,F_SETFL,fcntl(kb,F_GETFL,0)|O_NDELAY);
        fcntl(0, F_SETFL, O_NONBLOCK| O_NDELAY);
	tcgetattr(0,&term); // 0 is stdin

	term.c_lflag &= ~ICANON;
	term.c_cc[VTIME]=0;
	term.c_cc[VMIN]=0;
	tcsetattr(0,TCSANOW,&term); // 0 is stdin

	initialized = true;
}

/****************************************************/

static char lastChar = -1;

/** Returns -1 if no data, else char from getchar **/
char getcharNoWait() {
	if(initialized == false) setNoDelay();
	if(lastChar >= 0) {
		char c = lastChar;
		lastChar = -1;
		return c;
	}
	return getchar();
}

bool isCharReady() {
	if(initialized == false) setNoDelay();
	if(lastChar >= 0) return true;
	lastChar = getchar();
	return (lastChar >= 0);
}



char* getsNoWait() {
	static char inBuffer[122];
	static int inBufferIndex = 0;

	while(1) { // terminated with returns

		char c= getcharNoWait();
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
}
