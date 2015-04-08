#include "rodos.h"
#include "demo_topics.h"
#include "mask.h"

static Application  receiverName("receiver 15", 1115);

struct JustPrint15 : public Subscriber {
    JustPrint15() : Subscriber(counter5, "justprint15") { }
    long put(const long topicId, const long len, const void* msg, const NetMsgInfo& netMsgInfo) {
        PRINTF("%s%ld", R15, *(long*)msg);
        return 1;
    }
} justPrint15;

