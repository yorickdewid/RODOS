/*
 * This file shall test whether the scheduling locking works.
 * Therefore two threads are created, both printing their IDs and in the first test yielding, in the second waiting in a loop for a timer interrupt.
 * Every 5th print they lock/unlock scheduling.
 * The expected pattern is that they print their IDs five times interleaved and then five times in a block.
 */

#include <rodos.h>


class Printer : public Thread {
public:
    int id;

    void activeWait(TTime t) {
        TTime wanted = NOW() + t;
        while(NOW() < wanted);
    }
public:
    Printer(int n) {
        id = n;
    }

    virtual void run() {

        bool schedulingLocked = false;
        int cnt = 0;
        if(id == 0)
            xprintf("Testing whether scheduler locking works for yield()\n\n");


        while(1) {
            xprintf("%d", id);					//print ID
	    FFLUSH();
            if(++cnt % 5 == 0) {				//every fifth loop
                if((schedulingLocked = !schedulingLocked))	//toggle scheduler locking
                    globalAtomarLock();				//lock
                else
                    globalAtomarUnlock();			//unlock
            }

            yield();						//yield. If scheduling was locked before, this function should return immediately
            if(cnt > 1000) {					//If we looped 1000 times, unlock scheduling and leave loop
                globalAtomarUnlock();
                break;
            }
        }


        if(id == 1) {
            xprintf("\n\n\nTesting whether scheduler locking works for timer interrupts\n\n");
        }


        while(1) {
            xprintf("%d", id);				//Do the same again but this time don't yield but wait for a timer interrupt to initiate the context switch
	    FFLUSH();
            FFLUSH();
            if(++cnt % 5 == 0) {
                if((schedulingLocked = !schedulingLocked))
                    globalAtomarLock();
                else
                    globalAtomarUnlock();
            }

            activeWait(250*MILLISECONDS);
            if(cnt > 1100) {					//Break after 100 loops because the waiting takes long
                globalAtomarUnlock();
                break;
            }
        }


        //TODO: Please write test for handling of timeToTryAgainToSchedule here. Set it somehow and try to call the scheduler

        if(id == 0)
            xprintf("\n\n\nIn all test cases you should see blocks of the same IDs. If you do, scheduler locking works.");

        FFLUSH();
    }
} p1(0), p2(1);
