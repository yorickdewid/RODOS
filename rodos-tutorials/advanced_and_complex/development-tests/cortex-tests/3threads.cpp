#include "rodos.h"

static Application module01("TestTime");

class TestTime : public Thread {
  int myId;
public:
  TestTime(int num) : Thread("waitfor") {myId = num; }
  void run(){
    int cnt=0;
    double  t0;
    while(1){
      cnt++;
      suspendCallerUntil(NOW() + 2*SECONDS);
      t0 = SECONDS_NOW();
      //printf("After 2 Seconds  : %12.9f %d\n", t0, cnt);
      //printf("Timenow = %lld\n", NOW());
      PRINTF(" (%d) runnint at   %ld\n", myId, (long)(NOW() / 1000) );
    }
  }
  void init() { xprintf("Waiting 2 seconds");}
};	

static TestTime testTime1(1);
static TestTime testTime2(2);
static TestTime testTime3(3); // mit 3 Threads geht es nicht mehr


