/*
 * Copyright (c) 2011 Uni Wuerzburg, Wuerzburg

 * All rights reserved.
 *
 * @author Michael Ruffer
 */
#include "hal/hal_uart.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


//#include "hw_udp.h"
#include "rodos.h"
#include <windows.h>



#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

//===============================================================================






class ReceiveWaiter : public Thread{
public:
	volatile bool waitingForData;
	IOcontext* context;
	void run();


};

class IOcontext {
public:
	UART_IDX idx;
	HAL_UART* hal_uart;
	HANDLE hHandle;
	volatile int   lastChar;
	long   lRdBufferSize;
	long   lRdBufferPos;
	ReceiveWaiter receiveWaiter;
};

void ReceiveWaiter::run(){

	unsigned char readChar;
	int lRead;

	waitingForData=false;

	while(1){


		suspendCallerUntil(END_OF_TIME);

		waitingForData=true;
		while (ReadFile(context->hHandle, &readChar, 1, (LPDWORD) &lRead, 0) == 0 && lRead == 0){
			suspendCallerUntil(NOW()+ 1*SECONDS);
		}

		context->lastChar=readChar;
		waitingForData=false;
		context->hal_uart->upCallDataReady();

	}


}





int uart_rxErrorStatus(int idx);




HAL_UART::HAL_UART(UART_IDX uartIdx){
	context = new IOcontext;


	context->idx = uartIdx;
	context->hal_uart = this;


}






/*
 * USART
 * - all USART will be initialized in 8N1 mode
 */
int HAL_UART::init(unsigned int iBaudrate) {


	char pchComPort[32];
	sprintf(pchComPort,"\\\\.\\COM%d\\", context->idx);

	  COMMTIMEOUTS cTimeouts;
	  HANDLE hSerial;
		DCB    cState;




	  context->hHandle       = 0;
	  context->lRdBufferPos  = 0;
	  context->lRdBufferSize = 0;

		/* connect to the port */
		hSerial = CreateFile (pchComPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
		if (hSerial == INVALID_HANDLE_VALUE) return -1;

		/* get port configuration */
		memset (&cState, 0, sizeof(DCB));
		if (!GetCommState(hSerial, &cState))
		{
			printf("Error getting the CommState: %ld\n", GetLastError());
			return -1;
		}

		cState.BaudRate = iBaudrate; // UART_BAUDRATE;
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
			printf("Error configuring the port: %ld\n", GetLastError());
			CloseHandle(hSerial);
			return -1;
		}

		if(!SetupComm(hSerial, 1024, 2048))
		{
			printf("Error setting up comm: %ld\n", GetLastError());
			return -1;
		}

		/* get the timeout settings */
		memset (&cTimeouts, 0, sizeof(COMMTIMEOUTS));
		if (!GetCommTimeouts(hSerial, &cTimeouts))
		{
			printf("Error Getting Timeouts: %ld\n", GetLastError());
			CloseHandle(hSerial);
			return -1;
		}

		cTimeouts.ReadIntervalTimeout = 1000;
		cTimeouts.ReadTotalTimeoutMultiplier = 0;
		cTimeouts.ReadTotalTimeoutConstant = 0;
		cTimeouts.WriteTotalTimeoutMultiplier = 0;
		cTimeouts.WriteTotalTimeoutConstant = 0;

		/* set the timeout settings */
		if(!SetCommTimeouts(hSerial, &cTimeouts))
		{
			printf("Error Configuring Port Timeouts: %ld\n", GetLastError());
			CloseHandle(hSerial);
			return -1;
		}

		context->hHandle = hSerial;


		context->receiveWaiter.resume();

	return 0;
}





int HAL_UART::config(UART_PARAMETER_TYPE type, int paramVal) {

	switch (type) {
		case UART_PARAMETER_BAUDRATE:
			if (paramVal > 0)
			{
				reset();
				init(paramVal);
			}
			else {return -1;}
			break;

		case UART_PARAMETER_HW_FLOW_CONTROL:
			return -1;

		case UART_PARAMETER_ENABLE_DMA:
			return -1;

		default:
			return -1;
	}

	return 0;
}



void HAL_UART::reset(){

	CloseHandle(context->hHandle);
}


int HAL_UART::read(char* buf, int size) {

	int bytesRed=0;
	int lRead;
	if(size <=0) return 0;

	if(context->receiveWaiter.waitingForData)
		return 0;

	if(context->lastChar >= 0){
		*buf = context->lastChar;
		context->lastChar = -1;

		buf++;
		size--;
		bytesRed++;
		if(size <=0) return bytesRed;
	}

	int availible =status(UART_STATUS_RX_BUF_LEVEL);

	if(size > availible) size=availible;


	if(availible>0){
		if (ReadFile(context->hHandle, buf, size, (LPDWORD) &lRead, 0) == 0)
			return bytesRed;

		bytesRed += lRead;
		return bytesRed;

	}

	context->receiveWaiter.resume();

	return bytesRed;

}


int HAL_UART::write(const char* buf, int size) {
	if(size <=0) return 0;

	  char *pchStr = (char *) buf;
	  int   iLen;
	  int retval;


		if((retval=WriteFile(context->hHandle, pchStr, size, (LPDWORD)&iLen, 0))){


			return iLen;

		}



	  return -1;
}




int HAL_UART::getcharNoWait() {
	char c = 0;

	if (read(&c,1) > 0)
	{
		return (int)c;
	}
	else
	{
		return -1;
	}
}


int HAL_UART::putcharNoWait(char c) {
	if(write(&c,1)>0){
		return c & 0xFF;
	}else{
		return -1;
	}
}


int HAL_UART::status(UART_STATUS_TYPE type) {


	switch (type)
	{
		case UART_STATUS_RX_BUF_LEVEL:
			DWORD not_used;
			COMSTAT cs;
			ClearCommError(context->hHandle, &not_used, &cs);
			return cs.cbInQue;

		case UART_STATUS_RX_ERROR:
			return -1;
		//...
		default: return -1;
	}
}


bool HAL_UART::isWriteFinished() {
		return true;
}


bool HAL_UART::isDataReady() {
	return (status(UART_STATUS_RX_BUF_LEVEL) > 0);
}







#ifndef NO_RODOS_NAMESPACE
}
#endif

