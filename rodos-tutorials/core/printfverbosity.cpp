#include "rodos.h"

// you will se notning! verbositi = 0;
// set to 100 for all prints
// or do not define (comment out) to geht the default vervosity = 100


int printfVerbosity = 0;  //Be aware: it is outside of namepsace RODOS


class TestPrint: public Thread {
public:
  void run(){
    int loopCnt = 0;

    PRINTF_CONDITIONAL(0, "\nDid you see? no printf from main!************\n");
    TIME_LOOP(1*SECONDS, 500*MILLISECONDS){
      if(loopCnt < 5) {
	      PRINTF("time= %3.9f loopcnt = %d\n", SECONDS_NOW(), loopCnt);
      } else {
         PRINTF_CONDITIONAL(10, " This prints shall come if loocpn >= 10. Loopcnt = %d\n", loopCnt);
      }
      printfVerbosity++;
      loopCnt++;
    }

  }

} testPrint;



