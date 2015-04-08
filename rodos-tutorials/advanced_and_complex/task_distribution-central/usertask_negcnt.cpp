#include "rodos.h"
#include "globals.h"
#include "mask.h"


/******************************/

static Application negCnt("negCnt", TASK_NEGCNT);

static CommBuffer<ContextNegCnt> contextNegCnt;
static Subscriber nameNotImportant(negCntTopic, contextNegCnt);


/*************************************************/


class NegCntController : public Thread {
public:
   NegCntController() : Thread("NegCntController") { }
   void run () {
	ContextNegCnt myCnt;
        AT(1*SECONDS); PRINTF("%s%d\n", NC, TASK_NEGCNT); 

	negCnt.isActivated = false; // wait orders from taskmanager

	/****************** from here the controll loop ***/

	TIME_LOOP(100*MILLISECONDS, 500*MILLISECONDS) {
	  if(!negCnt.isActivated) { // still sleeping 
              PRINTF(SCREEN_MOVE_CURSOR_TO, 4, 15);
              PRINTF("%s-------\n", NEG); // \n just to force fflush
              continue;
          }     


	  contextNegCnt.get(myCnt);
	  myCnt.cnt--;
	  PRINTF(SCREEN_MOVE_CURSOR_TO, 4, 15);
	  PRINTF("%s%7lld\n", NEG, myCnt.cnt); // \n just to force fflush
	  negCntTopic.publish(myCnt);
	}
   }
} negCntController;


