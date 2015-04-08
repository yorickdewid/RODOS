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
#undef putchar


#include "putter.h"

class UART : public Putter {

  bool charReady;
  char lastChar;

  public:
    UART(const char* devname);
    virtual ~UART();

    void writechar(const char c); // SergioM
    bool isCharReady();
    bool getcharNoWait(char &c);
    virtual bool putGeneric(const unsigned int len, const void* msg);
};


#endif /* __HW_UART_H__ */
