#include "rodos.h"

static Application module01("Testgetchar");

class TestGets : public Thread {
public:
    TestGets() : Thread("testgetchar") { }
    void run() {
        char* s;
        PRINTF("Please type string of characters. Run at least 40 seconds\n");

        activateTopicCharInput();  //<........ THIS IS IT!!!!

        TIME_LOOP(30*SECONDS, 1*SECONDS) { 
            PRINTF("I call getsnowait -> topic deactivated! \n");
            if((s = getsNoWait())!= 0 ) { PRINTF("\n\n********* String: %s'%s'%s **********\n", SCREEN_GREEN, s, SCREEN_RESET); }
        }
    }
} testGets;

/******************************/

class CharReceiver :  public Subscriber {
public:
    CharReceiver() : Subscriber(charInput, "CharReceiver") { }

    void putFromInterrupt(const long topicId, const void* data, int len) {
        GenericMsgRef* msg = (GenericMsgRef*)data;
        msg->msgPtr[msg->msgLen] = 0;
        xprintf("\n Async: %d %s\n", msg->msgLen, msg->msgPtr); // no PRINTF in interrupts (Sempahore)
    }
} charReceiver;

