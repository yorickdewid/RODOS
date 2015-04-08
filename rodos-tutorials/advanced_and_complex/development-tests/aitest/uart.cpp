// -*- Mode: C++ -*-

///
// @file
// @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
// Test program to send 0s on one UART and 1s on the other.

#include <rodos.h>

class ThreadSender0: public Thread {
  UART uart;
public:
  ThreadSender0():
    Thread ("Sender 0", 100),
    uart   ("UART0")
  {}

  void init() {}

  void run() {
    while(1) {
      uart.putString("0");
      suspendCallerUntil(NOW() + 100*MILLISECONDS);
    }
  }
};

class ThreadSender1: public Thread {
  UART uart;
public:
  ThreadSender1():
    Thread ("Sender 1", 100),
    uart   ("UART1")
  {}

  void init() {}

  void run(){
    while(1) {
      uart.putString("1");
      suspendCallerUntil(NOW() + 100*MILLISECONDS);
    }
  }
};

static Application   application("UART Send Test", 4711);
static ThreadSender0 sender0;
static ThreadSender1 sender1;
