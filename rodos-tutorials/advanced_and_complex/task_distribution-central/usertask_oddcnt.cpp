#include "rodos.h"
#include "globals.h"
#include "mask.h"


/******************************/

static Application oddCnt("oddCnt", TASK_ODDCNT);

static CommBuffer<ContextOddCnt> contextOddCnt;
static Subscriber nameNotImportant(oddCntTopic, contextOddCnt);


/*************************************************/


class OddCntController : public Thread {
public:
   OddCntController() : Thread("OddCntController") { }
   void run () {
	ContextOddCnt myCnt;
        AT(1*SECONDS); PRINTF("%s%d\n", OC, TASK_ODDCNT); 

	oddCnt.isActivated = false; // wait orders from taskmanager

	/****************** from here the controll loop ***/

	TIME_LOOP(200*MILLISECONDS, 500*MILLISECONDS) {
	  if(!oddCnt.isActivated) { // still sleeping 
	  	PRINTF("%s-------\n", ODD);
		continue;
	  }

	  contextOddCnt.get(myCnt);
	  if(myCnt.cnt == 0) myCnt.cnt  = 1;
	  else               myCnt.cnt += 2;

	  PRINTF("%s%7lld\n", ODD, myCnt.cnt); // \n just to force fflush
	  oddCntTopic.publish(myCnt);
	}
   }
} oddCntController;


