#include "rodos.h"
#include "monitoringMsg.h"
#include "monitorprintf.h"

/******************************/

static Application senderApp04("sender04");

class Sender04 : public Thread {
public:
	Sender04() : Thread("Sender04") { }

  /***********************/
   void run () {
    int myCnt = 0;
	int lastBit = 0;

	TIME_LOOP(200*MILLISECONDS, 1*SECONDS) {
		myCnt++;
		lastBit = myCnt % 2;
		reportf("report 04 %d %s\n", myCnt+4, lastBit? "odd":"even");
		reportf("test 04 %c %d %ld %lld %s\n", 't',(int)1,(long)1234,NOW(),"test");
//		PRINTF("PRINTF04 %d %s\n", myCnt, lastBit? "odd":"even");
	}
   }
}sender04;

