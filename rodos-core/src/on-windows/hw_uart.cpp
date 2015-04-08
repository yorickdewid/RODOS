

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

#include "hw_uart.h"
#include <windows.h>
#include <stdio.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


#define UART_PORT_NR      1
#define UART_BAUDRATE  9600

// ***************************************************************************

typedef struct
{ HANDLE hHandle;
  char   achRdBuffer[128];
  long   lRdBufferSize;
  long   lRdBufferPos;
} t_HW_UART_OnWindows;

// ***************************************************************************

void UART::Init (char *pchComPort, long lBaudrate)
{	t_HW_UART_OnWindows *pcUART = new t_HW_UART_OnWindows;

  COMMTIMEOUTS cTimeouts;
  HANDLE hSerial;
	DCB    cState;
	
  fd        = 0;
  charReady = false;
  lastChar  = 0;
  context   = (void *) pcUART;

  pcUART->hHandle       = 0;
  pcUART->lRdBufferPos  = 0;
  pcUART->lRdBufferSize = 0;

	/* connect to the port */
	hSerial = CreateFile (pchComPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (hSerial == INVALID_HANDLE_VALUE) return;

	/* get port configuration */
	memset (&cState, 0, sizeof(DCB));
	if (!GetCommState(hSerial, &cState))
	{
		printf("Error getting the CommState: %d\n", GetLastError());
		return;
	}

	cState.BaudRate = lBaudrate; // UART_BAUDRATE;
	cState.ByteSize = 8;
	cState.DCBlength = sizeof(DCB);
	cState.fAbortOnError = 0;
	cState.fDsrSensitivity = 0;
	cState.fDtrControl = 0;
	cState.fInX = 0;
	cState.fNull = 0;
	cState.fOutX = 0;
	cState.fOutxCtsFlow = 0;
	cState.fOutxDsrFlow = 0;
	cState.fParity = 0;
	cState.fRtsControl = 0;
	cState.fTXContinueOnXoff = 1;
	cState.Parity = NOPARITY;
	cState.StopBits = ONESTOPBIT;
	cState.XoffLim = 0;
	cState.XonLim = 0;

	if(!SetCommState(hSerial, &cState))
	{
		printf("Error configuring the port: %d\n", GetLastError());
		CloseHandle(hSerial);
		return;
	}

	if(!SetupComm(hSerial, 1024, 2048))
	{
		printf("Error setting up comm: %d\n", GetLastError());
		return;
	}

	/* get the timeout settings */
	memset (&cTimeouts, 0, sizeof(COMMTIMEOUTS));
	if (!GetCommTimeouts(hSerial, &cTimeouts))
	{
		printf("Error Getting Timeouts: %d\n", GetLastError());
		CloseHandle(hSerial);
		return;
	}

	cTimeouts.ReadIntervalTimeout = 0;
	cTimeouts.ReadTotalTimeoutMultiplier = 0;
	cTimeouts.ReadTotalTimeoutConstant = 0;
	cTimeouts.WriteTotalTimeoutMultiplier = 0;
	cTimeouts.WriteTotalTimeoutConstant = 0;

	/* set the timeout settings */
	if(!SetCommTimeouts(hSerial, &cTimeouts))
	{
		printf("Error Configuring Port Timeouts: %d\n", GetLastError());
		CloseHandle(hSerial);
		return;
	}

  pcUART->hHandle = hSerial;
}

// ***************************************************************************

UART::UART(const char *devname) 
{ char achComPort[64];

//sprintf(achComPort,"\\\\.\\COM%i\0", UART_PORT_NR);
  sprintf(achComPort,"\\\\.\\COM%s\0", devname);

  Init (achComPort, 9600);
}

// ***************************************************************************

UART::UART(long lPort, long lBaudrate)
{ char achComPort[64];

  sprintf(achComPort,"\\\\.\\COM%ld\0", lPort);

  Init (achComPort, 9600);
}

// ***************************************************************************

UART::~UART() 
{ t_HW_UART_OnWindows *pcUART = (t_HW_UART_OnWindows *) context;

  CloseHandle (pcUART->hHandle);
}

// ***************************************************************************

void UART::writechar(const char c) 
{ t_HW_UART_OnWindows *pcUART = (t_HW_UART_OnWindows *) context;
  char achBuffer[16] = { c, 0 };
  int  len;

	WriteFile (pcUART->hHandle, achBuffer, 1, (LPDWORD)&len, 0);
}

// ***************************************************************************

bool UART::isCharReady() 
{	t_HW_UART_OnWindows *pcUART = (t_HW_UART_OnWindows *) context;
  DWORD not_used;
	COMSTAT cs;

	ClearCommError(pcUART->hHandle, &not_used, &cs);

  return (cs.cbInQue) ? true : false;
}

// ***************************************************************************

bool UART::getcharNoWait (char &c) 
{ t_HW_UART_OnWindows *pcUART = (t_HW_UART_OnWindows *) context;
  int lRead;

  if (pcUART->lRdBufferSize <= 0){
		if (ReadFile(pcUART->hHandle, pcUART->achRdBuffer, 128, (LPDWORD) &lRead, 0) == 0)
		  return false;

	  pcUART->lRdBufferPos  = 0;
	  pcUART->lRdBufferSize = lRead;
  }

  c = pcUART->achRdBuffer[(pcUART->lRdBufferPos)++];
  (pcUART->lRdBufferSize)--;

	return true;
}

// ***************************************************************************

bool UART::putGeneric(const unsigned int len, const void* msg) 
{ t_HW_UART_OnWindows *pcUART = (t_HW_UART_OnWindows *) context;
  char *pchStr = (char *) msg;
  int   iLen;

	WriteFile (pcUART->hHandle, pchStr, (int) len, (LPDWORD)&iLen, 0);

  return true;
}

// ***************************************************************************


#ifndef NO_RODOS_NAMESPACE
}
#endif

