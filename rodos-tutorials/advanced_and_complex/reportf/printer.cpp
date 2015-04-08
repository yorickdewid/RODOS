#include "rodos.h"
#include "monitorprintf.h"
#include "monitoringMsg.h"

/****** Shortcuts ***/


static Application  nameNotImportant00("Monitor");

struct ReportPrinter : public Subscriber {
    ReportPrinter() : Subscriber(monitorMsgTopic, "ReportPrinter") { }
    long put(const long topicId, const long len, const void* msg, const NetMsgInfo& netMsgInfo) {
        MonitoringMsg* report = (MonitoringMsg*)msg;

		if(report->msgType != MonitorPrintf::PRINTFMSG) return false;

		char* fmt = (char*) report->deserializeLong();

		// print from the data that it get by using MonitorPrintf
		MonitorPrintf receiverprintf;
		receiverprintf.printfFromMonitorMsg(fmt,report);

		return 1;
    }
        
} reportPrinter;
