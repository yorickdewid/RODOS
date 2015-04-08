#include "rodos.h"
#include "monitoringMsg.h"
#include "monitorprintf.h"

/******************************/

static Application senderApp03("sender03");

class Sender03 : public Thread {
public:
	Sender03() : Thread("Sender03") { }

   void run () {
    int myCnt = 0;
	int lastBit = 0;

	TIME_LOOP(400*MILLISECONDS, 1*SECONDS) {
		myCnt++;
		lastBit = myCnt % 2;
		reportf("report 03 %d %s\n", myCnt+3, lastBit? "odd":"even");
		reportf("test 03 %c %d %ld %lld %s\n", 't',(int)1,(long)1234,NOW(),"test");
//		PRINTF("PRINTF03 %d %s\n", myCnt, lastBit? "odd":"even");
	}
   }
}sender03;

