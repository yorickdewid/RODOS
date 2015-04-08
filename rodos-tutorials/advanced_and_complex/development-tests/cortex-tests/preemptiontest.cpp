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

  void run(){
    long long cnt = 0;
    while(1) {
      cnt++;
      if (cnt % 1000 == 0) {
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







