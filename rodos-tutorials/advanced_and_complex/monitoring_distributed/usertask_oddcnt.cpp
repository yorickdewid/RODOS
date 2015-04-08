#include "rodos.h"
#include "globals.h"
#include "monitoringMsg.h"


/******************************/

static Application oddCnt("oddCnt", TASK_ODDCNT);

static MonitoringMsg monitoring(TASK_ODDCNT);

class OddCntController : public Thread {

public:
   OddCntController() : Thread("OddCntController") { }
  
   void run () {

    double myCnt = 1;
	long cnt = 0;

	/****************** from here the controll loop ***/

	TIME_LOOP(200*MILLISECONDS, 1*SECONDS) {
		cnt++;
		monitoring.serialize(cnt);
		myCnt += 2;
		monitoring.serialize(myCnt);
		monitoring.report(ODD_CNT);
	}
   }
} oddCntController;


