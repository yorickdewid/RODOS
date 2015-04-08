/**
* @file externalevents01.cpp
* @date 2008/06/11 16:07
* @author Sergio Montenegro
*
* Copyright 2008 DLR
*
* @brief test of external event management
*/

#include "rodos.h"

// Announce application to the system inventory
static Application module01("externalEvents");


class SigTermHandler :  public Subscriber {
public:
    SigTermHandler() : Subscriber(interruptSigterm, "simplesub") { }
    void putFromInterrupt(const long topicId, const void* any, int len) {
        xprintf("external Event at %3.9f datalen %d\n",  SECONDS_NOW(), len);
    }
} sigTermHandler;


/// Class to add a simple event handler to the system.

class MyThread : public Thread {
    void run() {
        PRINTF("In other window call killall tst\n");
        TIME_LOOP(0, 1*SECONDS) {
            PRINTF("timeloop at %3.9f, in other window please call killall tst\n", SECONDS_NOW());
        }
    }
} myThread;
