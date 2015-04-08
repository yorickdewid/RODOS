#include "rodos.h"

static Application applic("ComBufTest");

CommBuffer<int> buf;

class Sender : public Thread {
   void run () {
    int  cnt = 0;
    while(1) {
      cnt++;
      PRINTF("Writing %d\n", cnt);
      buf.put(cnt);
      suspendCallerUntil(NOW() + 3*SECONDS);
    }
  }
};

class Receiver : public Thread {
  void run () {
    int cnt;
    while(1) {
      buf.get(cnt);
      PRINTF("Reading %d\n", cnt);
      suspendCallerUntil(NOW() + 2*SECONDS);
    }
  }
};

/******************************/

Sender   sender;
Receiver receiver;
