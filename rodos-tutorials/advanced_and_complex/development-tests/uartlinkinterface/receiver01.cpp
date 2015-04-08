#include "rodos.h"
#include "demo_topics.h"

static Application  receiverName("ReciverSync", 1400);

static SyncFifo<long, 5> fifo;
static Subscriber nameNotImportant02(echo, fifo);

char outStr[10];
class ReceiverSync : public Thread {
  void run () {
	long fromMwSw = 0;
    while(1) {
      fifo.syncGet(fromMwSw);
	  memcpy(outStr, &fromMwSw, 4);
	  outStr[4] = 0;
       PRINTF("************Receiver 02 gets from mw sw %ld = %s\n", fromMwSw, outStr);
    }
  }
};

/******************************/

static ReceiverSync receiverSync;
