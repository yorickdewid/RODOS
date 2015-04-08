#include "rodos.h"
#include "globals.h"
#include "monitoringMsg.h"

/******************************/

static Application oddEvenCnt("oddEvenCnt", TASK_ODDEVENCNT);

static MonitoringMsg monitoring(TASK_ODDEVENCNT);

class OddEvenCntController : public Thread {
public:
   OddEvenCntController() : Thread("OddEvenCnt") { }

  /***********************/
   void run () {
    int myCnt = 0;
	int lastBit = 0;

	TIME_LOOP(200*MILLISECONDS, 1*SECONDS) {
		myCnt++;
		lastBit = myCnt % 2;
		monitoring << myCnt; 
		if(lastBit == 0) {
			monitoring << (char*)"even" << lastBit << EOM(ODDEVEN_CNT);
		} else {
			monitoring << (char*)"odd" << lastBit << EOM(ODDEVEN_CNT);
		}

	}
   }
} oddEvenController;


