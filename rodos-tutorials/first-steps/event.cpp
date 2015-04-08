#include "rodos.h"

static Application module01("resumefromEvent");

class TestWaiter: public Thread {
public:
  void run(){
    while(1){
      PRINTF("Suspend and wait until some one resumes me\n");
      AT();
      PRINTF("testwaiter running again at %3.9f\n", SECONDS_NOW());
    }
  }
};	


static TestWaiter testWaiter;


class TimeEventTest : public TimeEvent {
public:
  void handle(){
    xprintf("   Time Event at %3.9f\n", SECONDS_NOW());
    testWaiter.resume();
    xprintf("   Testwaiter resumed from me\n");
  }

  void init() { activatePeriodic(5*SECONDS, 3*SECONDS); }
};

static TimeEventTest te01;
