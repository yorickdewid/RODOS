#include "rodos.h"
#include "scheduler.h"

static Application module02("PriorityCeiling", 3000);

volatile long counter = 0;

long max = 0;

class PriorityCeiler: public Thread {
public:
  PriorityCeiler(): Thread("PriotyCeiler", 20) {
  }

  void run() {
     max = getSpeedKiloLoopsPerSecond() > 100000 ?  100000000 : 1000000;
     suspendCallerUntil(1*SECONDS);
     PRIORITY_CEILING {
       xprintf("at %3.9f: thread started -", SECONDS_NOW());
       xprintf("- scheduleCounter= %ld\n", (long) Scheduler::getScheduleCounter());
     }

     for(long i=0; i < max; i++){
        PRIORITY_CEILING {
          counter++;
        }
     }

     PRIORITY_CEILING {
       xprintf("at %3.9f\n", SECONDS_NOW());
       xprintf("counter= %ld ", counter);
       xprintf("- scheduleCounter= %ld\n", (long) Scheduler::getScheduleCounter());
     }
    
     while(1) {
       suspendCallerUntil();
     }
  }
};


/******************/

PriorityCeiler     pc1, pc2;

/******************/

void MAIN() {

     xprintf("\nThis program tests the priority ceiling macro.\n");
     xprintf("Two threads continually increment a counter.\n");
     xprintf("The final count must be 200...00\n");
     xprintf("It takes about 25 seconds in LM8962.\n");
     xprintf("It takes about 4 seconds in Linux with Core i5 processor (but counts 100 times more).\n");
 
}
