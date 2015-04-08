

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
 * @author Lutz Dittrich, Sergio Montenegro, Olaf Frauenberger
 *
 * Copyright 2008 DLR
 *
 * @brief UART communication
 *
 */

// FIXME this is posix and should not be here
#include <stdio.h>

#include "debug.h"
#include "hw_uart.h"
#include "hw_uart_conf.h" // TODO a configuration file to configure the serial port
#include "serial.h"       // FreeRTOS definitions for serial port


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

// TODO Original line was this UART::UART(const int devNum)
UART::UART(const char* devname) {

#if 1
  // Initialise port using minimal definitions used for PC 
  // TODO but where do we name it?
  //comPort = (void *) xSerialPortInitMinimal(ser9600, // ulWantedBaud
	//				    sizeof(char) );	     // uxQueueLength

#else
  // Extended definitions for a serial port used by some architectures in FreeRTOS.
  comPort = (void *) xSerialPortInit( UARTconf:ePort, 
				      UARTconf:eWantedBaud, 
				      UARTconf:eWantedParity, 
				      UARTconf:eWantedDataBits, 
				      UARTconf:eWantedStopBits, 
				      UARTconf:uxBufferLength );
#endif
}

UART::~UART() {
  // Close port on exit.
  vSerialClose( (xComPortHandle) comPort );
}


void UART::writechar(const char c) {

  // signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed charcOutChar, portTickType xBlockTime );
  if (pdFAIL == xSerialPutChar( (xComPortHandle) comPort, 
				(signed) c, 
				( (portTickType) timeOut / portTICK_RATE_MS) ) ) {
    // TODO Value has not been sent
    ERROR("Cant write to serial port!") ;
  }

}

bool UART::putGeneric(const unsigned int len, const void* msg) {
  // void vSerialPutString( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength );

	unsigned int ctr;
	for (ctr = 0; ctr<len; ctr++) {
		writechar(((const char*)msg)[ctr]);
	}
	return true;


}


bool UART::isCharReady() {

  bool retValue = false;

  if (charReady) {
    // The last check was already positive.
    retValue = true;
  } else {
    // Check if there is a new valu available from the port
    //  signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, portTickType xBlockTime );
    
    signed char pcRxedChar; // temporary value for character to read

    if ( pdTRUE == xSerialGetChar( (xComPortHandle) comPort, 
				   &pcRxedChar, 
				   0) ) {  // xBlockTime: do not wait
      // A new character is available, buffer it.
      charReady = true;
      lastChar = pcRxedChar;
      retValue = true;
    } else {
      // No value to be read.
      retValue = false;
    }
  } // end if else charReady

  return retValue;

}



bool UART::getcharNoWait(char &c) {

  bool retValue = false;
  if(charReady) {
    // Previous check IsCharReady() has returned true,
    // now return also the value read during the check.
    charReady = false;
    c = lastChar;
    retValue = true;
  } else {
    // There was previously no input. Check if new input is available.

    //  signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, portTickType xBlockTime );
    
    signed char pcRxedChar; // temporary value for character to read

    if ( pdTRUE == xSerialGetChar( (xComPortHandle) comPort, 
				   &pcRxedChar, 
				   0) ) {  // xBlockTime: do not wait
      
      // A new character is available, return it.
      c = pcRxedChar;
      retValue = true;
    }
    // else  No value to be read, return false
  } // end if else charReady

  return retValue;

}


#ifndef NO_RODOS_NAMESPACE
}
#endif

