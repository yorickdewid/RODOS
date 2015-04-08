#include "rodos.h"
#include "globals.h"
#include "monitoringMsg.h"


/******************************/

static Application negCnt("negCnt", TASK_NEGCNT);

static MonitoringMsg monitoring(TASK_NEGCNT);

class NegCntController : public Thread {

public:
   NegCntController() : Thread("NegCntController") { }
  
   void run () {

    int myCnt = 0;
	int cnt = 0;

	/****************** from here the controll loop ***/

	TIME_LOOP(200*MILLISECONDS, 1*SECONDS) {
		cnt++;
		myCnt--;
		monitoring.report(NEG_CNT);
	}
   }
} negCntController;


