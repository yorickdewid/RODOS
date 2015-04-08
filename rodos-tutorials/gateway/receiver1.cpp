#include "rodos.h"
#include "demo_topics.h"

struct JustPrint1 : public Subscriber {
    JustPrint1() : Subscriber(counter1, "receiver1") { }
    long put(const long topicId, const long len, const void* msg, const NetMsgInfo& netMsgInfo) {
        PRINTF("at %9.3f       counter1: got (topic, len, mesg, netinfo): %ld, %ld, %d, (%d, %lld, %d, %d)\n", SECONDS_NOW(),
               topicId, len, *(int32_t*)msg,
               netMsgInfo.senderNode, netMsgInfo.sentTime, netMsgInfo.senderThreadId, netMsgInfo.linkId);
        return true;
    }
} justPrint1;

