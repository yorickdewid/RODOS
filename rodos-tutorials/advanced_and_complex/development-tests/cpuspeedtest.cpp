#include "rodos.h"


static const long NUM_OF_LOOPS  = 1000 * 1000 * 10;  
volatile long long cnt = 0;


class SpeedTest : public Thread {
public:
    SpeedTest() : Thread("SpeedTest", 10) { }

    void run() {

        /** how many loops in a second? **/
        xprintf("--------------------- %ld loops: \n", NUM_OF_LOOPS);
        long long startTime = NOW();
        for(cnt = 0; cnt < NUM_OF_LOOPS; cnt++)
            ;

        long long deltaTime = (NOW() - startTime);
        long long timeFor1M = deltaTime * 1000 * 1000 / NUM_OF_LOOPS;

        xprintf("time for %ld lops = %3.9f\n", NUM_OF_LOOPS, (double)deltaTime/SECONDS);
        xprintf("time for 1M lops = %3.9f\n",  (double)timeFor1M/SECONDS);

        long kiloLoopsPerSecond = (double)SECONDS * (double)NUM_OF_LOOPS / (double)deltaTime / 1000.0;
        xprintf("your CPU perfoms %ld Kilo loops per second\n", kiloLoopsPerSecond);
        xprintf("Please write in src/.../yourArch/hostinfos.cpp\n\n\n");
        xprintf("long getSpeedKiloLoopsPerSecond() { return %ld; } \n", kiloLoopsPerSecond);
        xprintf("------------------------------------------ done\n");
    }

} speedTest;









