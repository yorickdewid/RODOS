#include "rodos.h"
#include "demo_topics.h"

static Application  receiverName("ReciverSync", 1400);

static SyncFifo<long, 5> fifo;
static Subscriber nameNotImportant02(counter1, fifo);

class ReceiverSync : public Thread {
  void run () {
    long cnt = 0;
    while(1) {
      fifo.syncGet(cnt);
      PRINTF("			Sync Receiver reads %d\n", cnt);
    }
  }
};

/******************************/

static ReceiverSync receiverSync;
