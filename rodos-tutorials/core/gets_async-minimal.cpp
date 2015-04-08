#include "rodos.h"

void MAIN() {
        PRINTF("please write some lines\n");
        activateTopicCharInput();  //<........ THIS IS IT!!!!
} 


class CharReceiver :  public Subscriber {
public:
    CharReceiver() : Subscriber(charInput, "CharReceiver") { }

    void putFromInterrupt(const long topicId, const void* data, int len) {
        GenericMsgRef* msg = (GenericMsgRef*)data;
        xprintf("\n Async: %d %s\n", msg->msgLen, msg->msgPtr); // no PRINTF in interrupts (Sempahore)
    }
} charReceiver;

