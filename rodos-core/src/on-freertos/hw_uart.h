

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
*
* @brief UART communication
*
*/

#ifndef __HW_UART_H__
#define __HW_UART_H__
#include "../api/putter.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class UART : public Putter {

  static const int timeOut = 20; /* maximum delay in milliseconds if a character can not be sent */

  int fd; /* file descriptor */
  void* comPort;  /* handle for port opened xComPortHandle pxPort */
  bool charReady;
  char lastChar;

  public:
    // TODO Original line was this   
    // UART(const int devNum);
  /** Constructor, TBA */
  UART(const char* devname);

  /** Desrtructor TBA function */
  ~UART();

  /** Writes a single character to the port. */
  void writechar(const char c);

  /** Function to indicate whether a new datum is available. */ 
  bool isCharReady();

  bool getcharNoWait(char &c);

  virtual bool putGeneric(const unsigned int len, const void* msg);
};


#ifndef NO_RODOS_NAMESPACE
}
#endif


#endif /* __HW_UART_H__ */
