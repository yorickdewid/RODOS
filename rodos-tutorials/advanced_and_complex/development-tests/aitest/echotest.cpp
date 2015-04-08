#include "rodos.h"

static Application module01("Echotest");

class TestGets: public Thread {
private:
  UART uart0;
  UART uart1;

public:
  TestGets() : Thread("echotest"), uart1("UART1"), uart0("UART0") { }

  void init() { PRINTF("Echo test");}

  void run(){
    char c,d;
    PRINTF("\n\nThis test will receive characters from UART1 and echos them to (this) UART0\n");
    d=0;
    while(1){
      if(uart0.getcharNoWait(c)) {
        d=(d+1)%10; uart0.putchar(d+0x30);
        uart0.putchar(c);
      }
    }
  }
};

static TestGets testGets;
