#include "rodos.h"
#include "globals.h"
#include "mask.h"


/******************************/

static Application primCnt("primCnt", TASK_PRIMCNT);

static CommBuffer<ContextPrimCnt> contextPrimCnt;
static Subscriber nameNotImportant(primCntTopic, contextPrimCnt);


/*************************************************/


class PrimCntController : public Thread {
public:
   PrimCntController() : Thread("PrimCntController") { }
   void run () {
	ContextPrimCnt myCnt;
	//PRINTF(SCREEN_MOVE_CURSOR_TO, 7, 1);
        AT(1*SECONDS); PRINTF("%s%d\n", PC, TASK_PRIMCNT); 

	primCnt.isActivated = false; // wait orders from taskmanager

	/****************** from here the controll loop ***/

	TIME_LOOP(400*MILLISECONDS, 500*MILLISECONDS) {

	  if(!primCnt.isActivated) { // still sleeping 
	  	PRINTF("%s-------\n", PRIM);
		continue;
	  }

	  contextPrimCnt.get(myCnt);

	  long long candidat = myCnt.cnt;
	  for(long long div = 1; div != candidat;) {
		candidat++;
		for(div = 2; div < candidat; div++) {
			if(candidat % div == 0) break;
		}
	  }
	  myCnt.cnt = candidat;
	  
	  PRINTF(SCREEN_MOVE_CURSOR_TO, 7, 15);
	  PRINTF("%s%7lld\n", PRIM, myCnt.cnt); // \n just to force fflush
	  primCntTopic.publish(myCnt);
	}
   }
} primCntController;


