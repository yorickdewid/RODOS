

#include "rodos.h"


/**
 * @file gatewayExample.h
 * @date 2009/05/01 7:07
 * @author Sergio Montenegro
 *
 * Copyright 2009 DLR
 *
 * @brief example of a gateway to/from stdout/stdin (no real network)
 *
 */


class LinkinterfaceExample: public Linkinterface  {

public:

	// void virtual void init(Gateway* owner_) { }

	bool getNetworkMsg(NetworkMessage &inMsg);
	bool isNetworkMsgReadyToRead();
	bool sendNetworkMsg(NetworkMessage &outMsg);

};



/************* Connection to the network, network and protocoll dependent ***/

bool LinkinterfaceExample::isNetworkMsgReadyToRead() {
	return isCharReady();
}

bool LinkinterfaceExample::getNetworkMsg(NetworkMessage &inMsg)	{
	char* s;
	long topic, content;
	if((s = getsNoWait())== 0) return false;

	/** Just as example: Deliver messages which contain only a long valud **/

	sscanf(s, "%ld %ld", &topic, &content);
	PRINTF("networkMsg receiver: '%s' sending topic %ld, content %ld\n", s, topic, content);

	inMsg.senderNode	= 0;
	inMsg.topicId		= topic;
	inMsg.sendedTime	= NOW();
	inMsg.sendAfter		= 0;
	inMsg.len			= 4;
	inMsg.setUserData	(&content, 4);

	return true;
}

bool LinkinterfaceExample::sendNetworkMsg(NetworkMessage &outMsg)	{
	static char outString[200];

	/** Just as example **/
	sprintf(outString, "%02dOUTMST'%2x %2x %2x %2x\n", outMsg.topicId, 
			outMsg.userDataC[0], outMsg.userDataC[1], outMsg.userDataC[2], outMsg.userDataC[3]);

	PRINTF("Msg to send: '%s'\n", outString);
	return true;
}




static LinkinterfaceExample linkinterfaceExample;
static Gateway gatewayExample(&linkinterfaceExample);

static class MyInitiator : public Initiator {
    void init() {
        xprintf("Forwaretopics initiator\n");
        TopicListReport toForward;
        toForward.numberOfTopics = 3;
        toForward.topicList[0] = 10;
        toForward.topicList[1] = 20;
        toForward.topicList[2] = 30;
        gatewayExample.setTopicsToForward(&toForward);
    }
} myInitiator;




