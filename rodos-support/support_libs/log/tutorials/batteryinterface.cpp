#include "rodos.h"
#include "example_topics.h"
#include "random.h"
#include "../api/log.h"
/******************************/

static Application senderName("BatteryInterface", 1100);

class BatteryInterface : public Thread {
public:
	BatteryInterface() : Thread("BatteryInterfaceThread") { }
	void run () {
		long batVol = 0; // mV
		long batCur = 0; // mA
		TIME_LOOP(2*SECONDS, 3*SECONDS) {
			batVol = 5000 + (randomTT800() % 500); // max +-500mV
			batCur = 1000 + (randomTT800() % 300); // max +-300mA
			battery_current.publish(batCur, true);
			battery_voltage.publish(batVol, true);
		}
	}
} batteryInterface;
