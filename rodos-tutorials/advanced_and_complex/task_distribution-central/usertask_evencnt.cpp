#include "rodos.h"
#include "globals.h"
#include "mask.h"

/******************************/

static Application evenCnt("evenCnt", TASK_EVENCNT);

static CommBuffer<ContextEvenCnt> contextEvenCnt;
static Subscriber nameNotImportant(evenCntTopic, contextEvenCnt);


/*************************************************/


class EvenCntController : public Thread {
public:
   EvenCntController() : Thread("EvenCntController") { }
   void run () {
	ContextEvenCnt myCnt;
        AT(1*SECONDS); PRINTF("%s%d\n", EC, TASK_EVENCNT); 

	evenCnt.isActivated = false; // wait orders from taskmanager

	/****************** from here the controll loop ***/

	TIME_LOOP(300*MILLISECONDS, 500*MILLISECONDS) {
	  if(!evenCnt.isActivated) { // still sleeping 
	  	PRINTF("%s-------\n",EVEN);
		continue;
	  }

	  contextEvenCnt.get(myCnt);
	  myCnt.cnt += 2;

	  PRINTF("%s%7lld\n", EVEN, myCnt.cnt); // \n just to force fflush
	  evenCntTopic.publish(myCnt);
	}
   }
} evenCntController;


