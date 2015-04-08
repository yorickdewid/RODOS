#include "rodos.h"
#include "scheduler.h"

static Application module("semaphore");

Semaphore protector;

volatile long counter = 0;

#define LINUX

long max = 0;

class TestThread: public Thread {
public:
  TestThread(): Thread("semaphoretest") {
  }

  void run() {
     // max = getSpeedKiloLoopsPerSecond() > 100000 ?  100000000 : 1000000;
     max =  1000000;
     suspendCallerUntil(1*SECONDS);
     protector.enter(); 
       xprintf("at %3.9f:  thread started ", SECONDS_NOW());
       xprintf("- scheduleCounter= %ld\n", (long) Scheduler::getScheduleCounter());
     protector.leave();

     for(long i=0; i < max; i++){
        protector.enter(); 
          counter++;
        protector.leave();
     }

     protector.enter(); 
       xprintf("at %3.9f:", SECONDS_NOW());
       xprintf("counter= %ld ", counter);
       xprintf("- scheduleCounter= %ld\n", (long) Scheduler::getScheduleCounter());
     protector.leave();
    
     while(1) {
       suspendCallerUntil();
     }
  }
};


/******************/

TestThread     t1, t2;

/******************/

void MAIN() {

     xprintf("\nThis program tests the semaphore operation.\n");
     xprintf("Two threads continually increment a counter.\n");
     xprintf("The final count must be 2000...000\n");
     xprintf("It takes about 100 seconds in LM8962.\n");
     xprintf("It takes about 15 seconds in Linux with Core i5 processor (but counts 100 times more).\n");
 
}
