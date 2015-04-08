/*
 * topicreporter.h
 *
 *  Created on: 28.03.2013
 *      Author: Dilger
 */

#pragma once

#include "thread.h"
#include "gateway/router.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class TopicReporter: public Thread {

    NetworkMessage topicMessage;

protected:
    TopicListReport topiclist;

    Gateway* gateways[MAX_NUMBER_OF_GATEWAYS_PER_ROUTER];
    int numberOfGateways;

public:
    TopicReporter(Gateway* gateway1=0, Gateway* gateway2=0, Gateway* gateway3=0, Gateway* gateway4=0);
    void run();

    virtual void sendListOfTopicsToNetwork();

    void addLocalTopics(TopicListReport& list);
    void addTopicsFromGateway(TopicListReport& list,Gateway* gateway);

    void sendToAllGateways(TopicListReport& list);
    void sendToGateway(TopicListReport& list, Gateway* gateway);

    void addGateway(Gateway* gateway);

    virtual ~TopicReporter() {}
};

#ifndef NO_RODOS_NAMESPACE
}
#endif

