/**
* @file hw_uart.h
* @date 2008/12/03 9:55
* @author Lutz Dittrich, Sergio Montenegro, Adriano Carvalho
*
* Copyright 2008 DLR
*
* @brief UART communication
*
*/

#ifndef __HW_UART_H__
#define __HW_UART_H__

#include "putter.h"
#include <addresses.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class UART : public Putter {
  public:
    UART(const char* devname);
    virtual ~UART();

    void writechar(const char c);
    bool isCharReady();
    bool getcharNoWait(char &c);
    virtual bool putGeneric(const unsigned int len, const void* msg);

  private:
    unsigned int isTxFull();
    unsigned int isRxEmpty();

};


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __HW_UART_H__ */
