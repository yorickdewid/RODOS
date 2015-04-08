#include "rodos.h"
#include "globals.h"
#include "monitoringMsg.h"


/******************************/

static Application primCnt("primCnt", TASK_PRIMCNT);

static MonitoringMsg monitoring(TASK_PRIMCNT);

class PrimCntController : public Thread {

public:
   PrimCntController() : Thread("PrimntController") { }
   int  myCnt;
   int  cnt;
   
  /***********************/
	void doMyJob() {
	  for(long div = 1; div != myCnt;) {
		myCnt++;
		for(div = 2; div < myCnt; div++) {
			if(myCnt % div == 0) break;
		}
	  }

	}

  /***********************/
   void run () {

    myCnt = 0;
	cnt = 0;

	/****************** from here the controll loop ***/

	TIME_LOOP(200*MILLISECONDS, 1*SECONDS) {
		cnt++;
		monitoring << cnt << myCnt << EOM(PRIM_CNT_PRE);
		doMyJob();
		monitoring << cnt << myCnt << EOM(PRIM_CNT_POST);
	}
   }
} primCntController;


