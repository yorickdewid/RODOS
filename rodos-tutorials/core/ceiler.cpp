#include "rodos.h"

static Application module02("PriorityCeiling", 3000);

class PriorityCeiler: public Thread {
public:
  PriorityCeiler(): Thread("PriotyCeiler", 20) {
  }

  void init() {
    xprintf(" ceiler = '+'");
  }


  void run() {
    suspendCallerUntil(3*SECONDS);
    int64_t aproxLoopsFor3Seconds = getSpeedKiloLoopsPerSecond() * 1500LL;
    int64_t aproxLoopsForPrint = aproxLoopsFor3Seconds / 80;

    xprintf("\nPriority ceiler 122 times normal\n");
    for (int64_t cnt = 0; cnt < aproxLoopsFor3Seconds; cnt++) {
      if(cnt % aproxLoopsForPrint == 0) {
	xprintf("-"); FFLUSH();           
      }
    }

    xprintf("\nPriority ceiler 122 tmes prio ceiling\n");
    PRIORITY_CEILING {
    	for (int64_t cnt = 0; cnt < aproxLoopsFor3Seconds; cnt++) {
    		if (cnt % aproxLoopsForPrint == 0) {
    			xprintf("-"); FFLUSH();           
    		}
    	}
    }

    xprintf("\nPriority ceiler 122 times normal\n");
    for (int64_t cnt = 0; cnt < aproxLoopsFor3Seconds; cnt++) {
      if(cnt % aproxLoopsForPrint == 0) {
        xprintf("-"); FFLUSH();           
      }
     }

    xprintf("\nPriority ceiler 122 tmes atomarLock\n");
    globalAtomarLock(); 
    	for (int64_t cnt = 0; cnt < aproxLoopsFor3Seconds; cnt++) {
    		if (cnt % aproxLoopsForPrint == 0) {
    			xprintf("-"); FFLUSH();           
    		}
    	}
    globalAtomarUnlock();

    xprintf("\nPriority ceiler 122 times normal\n");
    for (int64_t cnt = 0; cnt < aproxLoopsFor3Seconds; cnt++) {
      if(cnt % aproxLoopsForPrint == 0) {
        xprintf("-"); FFLUSH();           
      }
     }


     xprintf("\nPriority ceiler terminates\n");
     while(1) {
       suspendCallerUntil();
     }
  }
};


/******************/

PriorityCeiler     priorityCeiler;

/******************/

void MAIN() {
  if (Application::findApplication(2000)==0) {
     xprintf("\n\n**********************\n");
     xprintf("To be able to test it compile together wirh PreemptionTest\n");
     xprintf("**********************\n\n\n");
  } else {
    xprintf("Preemptiontest found\n");
  }
}
