/**
 * @file hw_uart.cpp
 * @date 2008/12/03 10:03
 * @author David Waleczek, Lutz Dittrich, Sergio Montenegro
 *         Adriano Carvalho
 *
 * Copyright 2008 DLR
 *
 * @brief UART communication
 *
 */

#include "hw_uart.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

UART::UART(const char* devname) {
  *(UART0_BASEADDR + uart_ctrlOffset) = 0x03;
}

UART::~UART() { }

void UART::writechar(const char c) {
  while (isTxFull()) { }
  *(UART0_BASEADDR + uart_txOffset) = c;
}

bool UART::isCharReady() {
  return !isRxEmpty();
}

bool UART::getcharNoWait(char &c) {
  if (isRxEmpty()) {
    c = -1;
    return false;
  }
  else {
    c = *(UART0_BASEADDR + uart_rxOffset);
    return true;
  }
}

bool UART::putGeneric(const unsigned int len, const void* msg) {
  unsigned int ctr;
  for (ctr = 0; ctr<len; ctr++) {
    writechar(((const char*)msg)[ctr]);
  }
  return true;
}

unsigned int UART::isTxFull() {
  return *(UART0_BASEADDR + uart_statOffset) & (1 << 3);
}

unsigned int UART::isRxEmpty() {
    return !(*(UART0_BASEADDR + uart_statOffset) & (1 << 0));
}

extern "C" int putchar(int c) {
  // blocking send
  while (*(UART0_BASEADDR + uart_statOffset) & (1 << 3));;
  *(UART0_BASEADDR + uart_txOffset) = c;

  return c;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

