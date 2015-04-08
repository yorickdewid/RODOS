#include "rodos.h"

/****** Use modulId 2000 just be be found by other example: ceiler **/

static Application module01("FPUTest", 2000);

class MultiplyAccThread: public Thread{
public:
  MultiplyAccThread(const char* name) : Thread(name, 25) {
  }

  void run() {
    xprintf("correct result: 0.1 * 1E6 = 1E5\n");
    while(1) {
      long long startTime = NOW();
      float sum = 0;
      float f1 = 0.03;
      float f2 = 0.1;
      for (int i=0;i<1000000;i++){
    	  sum += f1*i+f2;
          sum -= f1*i;
      }
      long long calcTime = (NOW()-startTime)/1000000;
      xprintf("%s: %f Time: %lldms\n",name,sum,calcTime);
    }
  }
};

MultiplyAccThread t1("t1"),t2("t2"),t3("t3");

/******************/


