#include "rodos.h"
#include "monitoringMsg.h"
#include "monitorprintf.h"

/******************************/

static Application senderApp02("sender02");

class Sender02 : public Thread {
public:
	Sender02() : Thread("Sender02") { }

  /***********************/
   void run () {
    int myCnt = 0;
	int lastBit = 0;

	TIME_LOOP(300*MILLISECONDS, 1*SECONDS) {
		myCnt++;
		lastBit = myCnt % 2;
		reportf("report 02 %d %s %c\n", myCnt+2, lastBit? "odd":"even",'n');
		reportf("test 02 %c %d %ld %lld %s\n", 't',(int)1,(long)1234,NOW(),"test");
//		PRINTF("PRINTF02 %d %s\n", myCnt, lastBit? "odd":"even");
	}
   }
}sender02;

