// -*- Mode: C++ -*-

///
// @file
// @author Christian Schulte (AbsInt Angewandte Informatik GmbH)
// The Sender transmitts smaller blocks (compared to the UART buffsize) of 
// 60 bytes of data on "UART0" which is wire-connected to "UART1" as fast 
// as possible
// The Receiver slowly fetches all incoming data on "UART1" thus producing
// a data jam in the uart buffers
//
//  

#include <rodos.h>

class ThreadSender: public Thread {
  UART uart;
public:
  ThreadSender():
    Thread ("Sender", 100),
    uart   ("UART0")
  {}

  void init() {}

  void run() {
    char sendstr[12]="0123456789\n";
    char ch=sendstr[0];
    unsigned char n=0;
    while(1) {
      while (uart.putCharWait(ch,END_OF_TIME)) {  // really do to send it if possible
        n = (n+1)%11;
        ch = sendstr[n];
      }
      uart.putCharWait('F',END_OF_TIME); // we should never get here
      Thread::yield();
    }
  }
};

class ThreadReceiver : public Thread {
  UART uart;
public:
  ThreadReceiver():
    Thread ("Receiver", 100),
    uart   ("UART1")
  {}

  void init() {}

  void run(){
    char sendstr[12]="0123456789\n";
    char ch=sendstr[0];
    unsigned char n=0;

    int c;
    while(1) {
      while (c=getcharNoWait() != -1) {  // read everything
        if (c!=ch) { 
          uart.putCharWait(c,NOW()+100*MILLISECONDS);            
          uart.putCharWait('\n',NOW()+100*MILLISECONDS);            
          while(true) {} 
        }
        n = (n+1)%11;
        ch = sendstr[n];
      }
      suspendCallerUntil(NOW() + 100*MILLISECONDS);
    }
  }
};

static Application      application("UART Buffer/Transceiver Test", 42);
static ThreadSender     sender;
static ThreadReceiver   receiver;

