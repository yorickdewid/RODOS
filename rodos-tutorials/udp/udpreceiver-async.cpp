
#include "rodos.h"

static Application  receiverName("UDPReceiverAsync");

Topic<GenericMsgRef> udpMessages(-1, "udpMessages");

static UDPIn in(5001);
static UDPIn nameNotImportatn01(5002, &udpMessages);
static UDPIn nameNotImportatn02(5003, &udpMessages);

void MAIN() {
    PRINTF("\n\n to test please type in other terminal\n");
    PRINTF("    netcat -u localhost 5001\n");
    PRINTF("    netcat -u localhost 5002\n");
    PRINTF("    netcat -u localhost 5003\n");
}

class ReceiverPolling : public Thread {
    char userData[2000];
    void run () {
        TIME_LOOP(1*SECONDS, 1*SECONDS) {
            PRINTF("\nPolling:");
            while(int len = in.get(userData, 2000)) {
                PRINTF("from polling: %d %s\n", len, userData);
            }
        }
    }
} receiverPolling;

/******************************/

class Udpreceiver :  public Subscriber {
public:
    Udpreceiver() : Subscriber(udpMessages, "udpReceiver") { }

    void putFromInterrupt(const long topicId, const void* data, int len) {
        GenericMsgRef* msg = (GenericMsgRef*)data;
        msg->msgPtr[msg->msgLen] = 0;
        xprintf("\n Async: %d %s\n", msg->msgLen, msg->msgPtr); // no PRINTF in interrupts (Sempahore)
    }
} udpReceiver;


