#include "rodos.h"

static Application module01("TestTime");

class TestTime : public Thread {
public:
    TestTime() : Thread("waitfor") { }
    void run() {
        int cnt=0;
        while(1) {
            cnt++;
            suspendCallerUntil();
            PRINTF(" %x, After 2 Seconds  : %3.9f %d\n", (int)this, SECONDS_NOW(), cnt);
        }
    }
};


TestTime t[4];


class MySchedluler : public Thread {
public:
    MySchedluler() : Thread("mySchedluler", 10) { }
    void run() {
        TIME_LOOP(1*SECONDS, 2*SECONDS) {
            PRINTF("-----------------------------------\n");
            for(int i = 0; i < 4; i++) {
                t[i].resume();
                yield();
            }
        }
    }
} mySchelduer;



