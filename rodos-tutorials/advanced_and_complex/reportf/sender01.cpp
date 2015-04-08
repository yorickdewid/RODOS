#include "rodos.h"
#include "monitoringMsg.h"
#include "monitorprintf.h"

/******************************/

static Application senderApp("sender01");

class Sender01 : public Thread {
public:
	Sender01() : Thread("Sender01") { }

  /***********************/
   void run () {
    int myCnt = 0;
	int lastBit = 0;

	TIME_LOOP(200*MILLISECONDS, 1*SECONDS) {
		myCnt++;
		lastBit = myCnt % 2;
		reportf("report 01 %d %s %d\n", myCnt+1, lastBit? "odd":"even", 1234);
		reportf("test 01 %c %d %ld %lld %s\n", 't',(int)1,(long)1234,NOW(),"test");
//		PRINTF("PRINTF01 %d %s\n", myCnt, lastBit? "odd":"even");
	}
   }
}sender01;

