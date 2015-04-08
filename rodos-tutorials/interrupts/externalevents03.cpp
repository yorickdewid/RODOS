/**
* @file timeevents.cc
* @date 2008/06/11 16:07
* @author Sergio Montenegro
*
* Copyright 2008 DLR
*
* @brief test of external event management
*/

#include "rodos.h"

static Application module02("externalEvents03");

class MyThread2 : public Thread {
    void run() {
        while(1) {
            PRINTF("In other Term, please call killall tst. Last msg at %3.9f\n", SECONDS_NOW());
            bool signalArrived = suspendCallerUntil(NOW()+ 5*SECONDS);
            if(!signalArrived) PRINTF("Timeout... still waiting (6 seconds) \n");
            else               PRINTF("Thanks, please again\n");
        }
    }
} myThread2;


// WARNING: this will resime MyThread, even if it is waiting in a semaphore!
class Resumer : public Putter {
    bool putGeneric(const long topicId, const unsigned int msgLen, const void* msg, const NetMsgInfo& netMsgInfo) {
        // from interrupt: no data (msg) expected
        myThread2.resume();
        return true;
    }
} resumer;

static Subscriber nameNotImportant03(interruptSigterm, resumer, "resumeFromInterrupt01");
static Subscriber nameNotImportant04(interruptTimer, resumer, "resumeFromTimer02");
static Subscriber nameNotImportant05(interruptUart, resumer, "resumeFromUartInterrupt");


