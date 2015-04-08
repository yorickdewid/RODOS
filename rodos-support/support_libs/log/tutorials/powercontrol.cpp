#include "rodos.h"
#include "example_topics.h"
#include "../api/log.h"

using namespace LOG;

static Application receiverName("PowerControl", 1100);

long batteryCurrent = 0;
long batteryVoltage = 0;

class PowerControlThread : public Thread, private LOG::Logger {
public:
    PowerControlThread() : Thread("PowerControlThread"), Logger(&receiverName) { }
    void run () {
    	long utcNowInSeconds;
    	while(true) {
    		suspendCallerUntil(NOW() + 1*SECONDS);
    		utcNowInSeconds = sysTime.getUTC() / 1000000000;
    		this->log(debug) << "Time since Boot (sec): " << utcNowInSeconds << endl;
    	}
    }
} powerControlThread;

class BatteryCurrentSubscriber :  public Subscriber, private LOG::Logger {
public:
	BatteryCurrentSubscriber() : Subscriber(battery_current, "PowerControl"), Logger(&receiverName) { }
        long put(const long topicId, const long len, const void* data,  const NetMsgInfo& netMsgInfo) {
            batteryCurrent = *(long*)data ;
		    this->log(info) << "Battery Current (mA): " << batteryCurrent << endl;
        return 1;
	}
} batteryCurrentSubscriber;

class BatteryVoltageSubscriber :  public Subscriber, private LOG::Logger {
public:
    BatteryVoltageSubscriber() : Subscriber(battery_voltage, "PowerControl"), Logger(&receiverName) { }
        long put(const long topicId, const long len, const void* data,  const NetMsgInfo& netMsgInfo) {
            batteryVoltage = *(long*)data;
            // the initial PRINTF statement:
            //PRINTF("PWR: len: %ld; data: %f topicId: %ld linkId %ld\n", len, batVol_calibrated/1000, topicId, linkId);

            // TODO: add calibration in groundstation logger
            //		  (the operator "<<" takes only integers currently)
            //PRINTF("PWR: Battery Voltage (V): %f\n", batteryVoltage/1000);
            this->log(info) << "Battery Voltage (mV): " << batteryVoltage << endl;
       return 1;
    }
} batteryVoltageSubscriber;

