#include "rodos.h"

/****** Use modulId 2000 just be be found by other example: ceiler **/

static Application module01("PreemptiveTest", 2000);

class HighPriorityThread: public Thread{
public:
  HighPriorityThread() : Thread("HiPriority", 25) { 
  }

  void init() { 
    xprintf(" hipri = '*'"); 
  }

  void run() {
    while(1) {
      xprintf("*");
      FFLUSH();
      suspendCallerUntil(NOW() + 500*MILLISECONDS);
    }
  }
};


class LowPriorityThread: public Thread {
public:
  LowPriorityThread() : Thread("LowPriority", 10) { 
  }

  void init() { 
    xprintf(" lopri = '.'"); 
  }

  void run() {
    int64_t cnt = 0;
    int32_t intervalToPrint = getSpeedKiloLoopsPerSecond() * 10;
    while(1) {
      cnt++;
      if (cnt % intervalToPrint == 0) {
        xprintf("."); 
	FFLUSH();
      }
    }
  }
};


/******************/

HighPriorityThread highPriorityThread;
LowPriorityThread  lowPriorityThread;

/******************/


