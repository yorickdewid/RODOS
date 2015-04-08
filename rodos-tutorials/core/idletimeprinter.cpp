#include "rodos.h"


class IdelCntPrinter: public Thread {
public:
  void run() {
    int64_t myIdleCnt = 0;
    for(int i = 0; i < 5; i++) {
        PRINTF("in Idle: idlecnt Per Seccond = %lld\n", idleCnt - myIdleCnt);
        myIdleCnt = idleCnt;
        suspendCallerUntil(NOW() + 1*SECONDS);
    }
    
    
    int64_t cnt = 0;
    int64_t timeLastPrint = NOW();

    myIdleCnt = idleCnt;
    while(1) {
        cnt++;
        if((NOW() - timeLastPrint) > 1*SECONDS) {
            timeLastPrint = NOW();
            PRINTF("full load: idlecnt Per Seccond = %lld\n", idleCnt - myIdleCnt);
            myIdleCnt = idleCnt;
        }
    }
    
  }
} idelCntPrinter;



