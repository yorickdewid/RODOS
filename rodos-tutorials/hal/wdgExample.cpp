/*
 * gpioExample.cpp
 *
 * Created on: 25.11.2013
 * Author: Michael Ruffer
 *
 */
#include "rodos.h"
#include "hal/hal_gpio.h"


static Application module01("HAL Watchdog example", 2000);

class WdgExample: public Thread {
	uint64_t periode;

public:
	WdgExample(const char* name, uint64_t periode) :
			Thread(name) {
		this->periode = periode;
	}


	void init() {
	    hwInitWatchdog(periode/MILLISECONDS*2);
	}


	void run() {
	    uint32_t i = 0;
		TIME_LOOP(0,periode){
		    hwTriggerWatchdog();
		    PRINTF("Triggering watchdog ...\n");
		    i++;
		    if (i>=10){
		        PRINTF("Stop triggering watchdog. Reset coming soon ...\n");
		        suspendCallerUntil();
		    }
		}
	}
};

WdgExample wdgThread("WdgExp",1000 * MILLISECONDS);
/***********************************************************************/
