#include "rodos.h"
#include "topics.h"

static Application  reciverSimple("ReciverSimple", 1100);

class SimpleSub :  public Subscriber {
public:
  SimpleSub() : Subscriber(counter1, "simplesub") { }
  long put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) {
    PRINTF("SimpleSub - Length: %ld Data: %ld TopicId: %ld \n", len, *(long*)data, topicId);
    return 1;
  }
} simpleSub; // try this: , a, b, c;

