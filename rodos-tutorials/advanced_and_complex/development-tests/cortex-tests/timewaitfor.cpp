#include "rodos.h"

static Application module01("TestTime");

class TestTime : public Thread {
public:
  TestTime() : Thread("waitfor") { }
  void run(){
    int cnt=0;
    while(1){
      cnt++;
      suspendCallerUntil(NOW() + 2*SECONDS);
      PRINTF("After 2 Seconds  : %3.9f %d\n", SECONDS_NOW(), cnt);
      PRINTF("Timenow = %lld\n", NOW());
    }
  }
  void init() { PRINTF("Waiting 2 seconds");}
};	

static TestTime testTime;


