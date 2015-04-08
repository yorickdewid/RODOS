#include "rodos.h"

class WdgTest: public Thread {
public:
    WdgTest():Thread("WdgTest"){}

	void init(){
		hwInitWatchdog(2000);
	}

	void run(){
		long long periode = 1000*MILLISECONDS;
		double lastTriggerTime = 0;
		xprintf("Watchdog Test\n");
		xprintf("Watchdog must be triggered at least once every 2 seconds\n\n");

		while (1){
			xprintf("Trigger Watchdog at:   %3.3f s\n", SECONDS_NOW());
			xprintf("Elapsed time:          %3.3f s\n\n", SECONDS_NOW()-lastTriggerTime);
			lastTriggerTime = SECONDS_NOW();
			hwTriggerWatchdog();

			periode += 100*MILLISECONDS;
			this->suspendCallerUntil(NOW() + periode);
		}
	}
};

WdgTest w1;
