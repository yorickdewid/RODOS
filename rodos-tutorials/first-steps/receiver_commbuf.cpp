#include "rodos.h"
#include "topics.h"

static Application receiverName("ReciverComBuffer", 1200);

static CommBuffer<long> buf;
static Subscriber receiverBuf(counter1, buf, "receiverbuf");

class ReceiverBuf : public Thread {
  void run () {
    long cnt;
    TIME_LOOP(0, 1.1*SECONDS) {
      buf.get(cnt);
      PRINTF( "ReciverComBuffer - counter1: %ld\n",cnt);
    }
  }
} recbuf;

