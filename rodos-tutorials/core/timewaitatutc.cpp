#include "rodos.h"

static Application module02("TestTimeAT");

static class TestTime : public Thread {

public:
  TestTime() : Thread("waitAT") { }
  void run(){
    sysTime.setUTC(100*SECONDS);
    PRINTF("Starting and waiting until 3th second\n");
    AT_UTC(103*SECONDS); PRINTF("after 3th second\n");
  }
} testTime;



