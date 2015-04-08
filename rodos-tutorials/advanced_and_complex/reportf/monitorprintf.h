#ifndef __MONITORPRINTF__
#define __MONITORPRINTF__

#include "stdarg.h"
#include "rodos.h"
#include "monitoringMsg.h"

extern "C" int putchar(int c);

/**
 * ReportPrinter uses micro printf (subset) to format the printing.
 * It print by send the data by MonitoringMsg.
 * Then at another side, the receiver can retrieve the printing
 * data from the MonitoringMsg and print it to prefer interface.
 *
 * Autor: Unkown (long long time ago)
 * c++, %ll, &b: Extended: Sergio Montenegro, 2011
 * MonitoringMsg Extended : Nawarat Termtanasombat , 2011
 *
 */
class MonitorPrintf {
public:

	/** ID for monitoring message sending, sender and receiver need to know this. */
	static const int PRINTFMSG = 101;
	/** ID for initial monitoringMsg object */
	static const int MONITORING_ID = 1001;

    ~MonitorPrintf() { }

    /******** for sender ******************/

    /** Monitoring message object, static because we need only one instance of it. */
    static MonitoringMsg monitoringMsg;

    /**
     * Format printing which send the data to the monitoringMsg.
     * This will be print out somewhere else where the printfFromMonitorMsg is called.
     */
    static void printfToMonitorMsg(const char *fmt, va_list ap);

    /********for receiver*****************/

    /** virtual function that can be override to print on many interface. */
    virtual void yputc(char c) { putchar(c); } // define yours *****

    /**
     * Format printing which retrieve the data from the monitoringMsg object.
     */
    void printfFromMonitorMsg(const char *fmt, MonitoringMsg* report);
};

/** reportf protected with a semaphore: Thread safe **/
void reportf(const char* fmt, ...) __attribute__((__format__(__printf__,1,2)));

#endif // __MONITORPRINTF__
