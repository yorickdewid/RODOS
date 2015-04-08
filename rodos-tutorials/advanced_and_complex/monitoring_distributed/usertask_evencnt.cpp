#include "rodos.h"
#include "globals.h"
#include "monitoringMsg.h"


/******************************/

static Application evenCnt("evenCnt", TASK_EVENCNT);

static MonitoringMsg monitoring(TASK_EVENCNT);

class EvenCntController : public Thread {

public:
   EvenCntController() : Thread("EvenCntController") { }
   
  /***********************/
   void run () {

    int myCnt = 0;
	int cnt = 0;

	/****************** from here the controll loop ***/

	TIME_LOOP(200*MILLISECONDS, 1*SECONDS) {
		cnt++;
		myCnt += 2;
		monitoring << cnt << myCnt << EOM(EVEN_CNT_POST);
	}
   }
} evenCntController;


