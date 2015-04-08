#include "rodos.h"
#include "demo_topics.h"
#include "mask.h"

static Application  receiverName("ReciverThread", 1500);

struct ReceiverThread :  public Subscriber,  public Thread  {

  CommBuffer<long> inputMsgBuffer;

  ReceiverThread() : Subscriber(counter1, "receiverthread") { }

  long put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) {
      PRINTF(PL "%ld" PT "%ld"  PD "%ld\n", len, topicId, *(long*)data);
      inputMsgBuffer.put(*(long*)data);
      this->resume();                         // not to publish from interrupt, call a thread to do it
      return 1;
   }           

  void run () {
    long cnt = 0;
    while(1) {
      suspendCallerUntil(); // the subscriver sahll reactivate it 
      inputMsgBuffer.get(cnt);
      PRINTF(RD "%ld\n", cnt);
    }
  }
} receiverThread;

