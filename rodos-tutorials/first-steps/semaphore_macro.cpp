#include "rodos.h"

Semaphore onlyOne;
char globalCnt = 'A';

class Watcher : public Thread {
public:

  void run() {

    char myId;

    //distribute the ids
    PROTECT_WITH_SEMAPHORE(onlyOne){
      myId = globalCnt++;
    }

    while(1) {
      PROTECT_WITH_SEMAPHORE(onlyOne){
        PRINTF(" only one, I am -- %c -- ", myId);
        yield(); // Just to show that the semaphore protects
        PRINTF("time %3.9f\n", SECONDS_NOW());
      }
    }

  }

} team[20];

