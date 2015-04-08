#include "rodos.h"
#include "demo_topics.h"

static Application  receiverName("ReciverSync", 1400);

static SyncFifo<Position, 5> fifo;
static Subscriber nameNotImportant02(position, fifo);

class ReceiverSync : public Thread {
  void run () {
    Position pos = {0,0};
    while(1) {
      fifo.syncGet(pos);
      PRINTF("			Sync Receiver reads %d %d\n", (int)pos.x, (int)pos.y);
    }
  }
};

/******************************/

static ReceiverSync receiverSync;
