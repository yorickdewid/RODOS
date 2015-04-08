#include "rodos.h"
#include "scheduler.h"

static Application module("semaphore");

Semaphore protector;

volatile long counter = 0;

#define LINUX

#ifdef LINUX
long max = 100000000;
#else
long max = 1000000;
#endif

class TestThread: public Thread {
public:
  char *name;
  TestThread(char *n): Thread("semaphoretest") { name = n; }

  void run() {
     suspendCallerUntil(1*SECONDS);
     protector.enter(); 
       xprintf("hier %s\n", name);
       xprintf("at %ld,%09ld: thread started ", SPLIT_SEC_NS(NOW()) ) ;
       xprintf("- scheduleCounter= %ld\n", (long) Scheduler::getScheduleCounter());
     protector.leave();

     for(long i=0; i < max; i++){
        protector.enter(); 
          counter++;
        protector.leave();
     }

     protector.enter(); 
       xprintf("hier %s\n", name);
       xprintf("at %ld,%09ld:", SPLIT_SEC_NS(NOW()));
       xprintf("counter= %ld ", counter);
       xprintf("- scheduleCounter= %ld\n", (long) Scheduler::getScheduleCounter());
     protector.leave();
    
     while(1) {
       xprintf(" %x, = %s beendet semaphore daten: \n", (int)this, name);
       //xprintf(" owenr %x, entercnt =%d\n", (int)protector.owner, (int)protector.ownerEnterCnt); // variables are private
       suspendCallerUntil();
     }
  }
};


/******************/

TestThread     t1((char*)"1111"), t2((char*)"22222");

/******************/

void MAIN() {

     xprintf("\nThis program tests the semaphore operation.\n");
     xprintf("Two threads continually increment a counter.\n");
     xprintf("The final count must be %ld\n", 2*max);
     //xprintf("It takes about 25 seconds in LM8962.\n");
     //xprintf("It takes about 4 seconds in Linux with Core i5 processor (but counts 100 times more).\n");
 
}
