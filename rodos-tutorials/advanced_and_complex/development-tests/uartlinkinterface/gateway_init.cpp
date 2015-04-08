
#include "rodos.h"

#define CHARS2INT(a,b) (((a)<<8) + (b))

LinkinterfaceUART linkinterfaceUART("/dev/ttyS0");

Gateway gatewayExample(&linkinterfaceUART); 

static class MyInitiator : public Initiator {
	void init() {
		xprintf("forward initiator\n");

		TopicListReport toForward;
		toForward.numberOfTopics = 3;
		toForward.topicList[0] = CHARS2INT('1', '1');
		toForward.topicList[1] = CHARS2INT('1', '2');
		toForward.topicList[2] = CHARS2INT('1', '3');
		gatewayExample.setTopicsToForward(&toForward);

	}
} myInitiator;


