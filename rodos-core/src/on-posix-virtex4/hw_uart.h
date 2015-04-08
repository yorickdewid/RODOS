

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
* @file hw_uart.h
* @date 2008/12/03 9:55
* @author Lutz Dittrich, Sergio Montenegro
*
* Copyright 2008 DLR
*
* @brief UART communication
*
*/

#ifndef __HW_UART_H__
#define __HW_UART_H__


#include "../api/putter.h"
#include "xparameters.h"
#include "xstatus.h"
#include "xuartlite.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

void getUARTStats(XUartLite_Stats*);
void clearUARTStats();


class UART : public Putter {

  int fd;  /* file descriptor */
  bool charReady;
  char lastChar;

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define UARTLITE_DEVICE_ID	XPAR_UARTLITE_0_DEVICE_ID

XUartLite UartLite;		/* Instance of the UartLite Device */

/*
 * The length of the buffers to be sent and received with the UartLite
 * must be 16 bytes or less since this is a single threaded 
 * non-interrupt driven example such that the entire buffer will fit 
 * into the transmit and receive FIFOs of the UartLite.
 */
u8 sendBuffer[1];	/* Buffer for Transmitting Data */
u8 recvBuffer[1];	/* Buffer for Receiving Data */

  public:
    UART(const char* devname);
    ~UART();
    void writechar(const char c);
    bool isCharReady();
    bool getcharNoWait(char &c);
    virtual bool putGeneric(const unsigned int len, const void* msg);
    void getStats(XUartLite_Stats*);
    void clearStats();
};




#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __HW_UART_H__ */
