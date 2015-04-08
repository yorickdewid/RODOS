/*
 * topicreporter.cpp
 *
 *  Created on: 28.03.2013
 *      Author: Dilger
 */

#include "rodos.h"

#include "gateway/topicreporter.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

#define not0(a) ((a)?1:0)

TopicReporter::TopicReporter(Gateway* gateway1, Gateway* gateway2, Gateway* gateway3, Gateway* gateway4) {
    gateways[0]      = gateway1;
    gateways[1]      = gateway2;
    gateways[2]      = gateway3;
    gateways[3]      = gateway4;
    numberOfGateways = not0(gateway1) + not0(gateway2) + not0(gateway3) + not0(gateway4);
}

void TopicReporter::run() {
    while(1) {
        topiclist.init();
        sendListOfTopicsToNetwork();
        suspendCallerUntil(NOW()+ 1LL*SECONDS);
    }
}

void TopicReporter::sendListOfTopicsToNetwork() {
    addLocalTopics(topiclist);
    sendToAllGateways(topiclist);
}


void TopicReporter::addLocalTopics(TopicListReport& list) {
    ITERATE_LIST(TopicInterface, TopicInterface::topicList) {
        if (iter->mySubscribers != 0) { // only topics with subscribers
            list.add((short)(iter->topicId));
        }
    }
}


void TopicReporter::addTopicsFromGateway(TopicListReport& list, Gateway* gateway) {
    TopicListReport* gatewayTopics = gateway->getTopicsToForward();

    for(int i=0; i<gatewayTopics->numberOfTopics; i++) {
        list.add(gatewayTopics->topicList[i]);
    }

}


void TopicReporter::sendToAllGateways(TopicListReport& list) {
    prepareNetworkMessage(topicMessage,0,&list,list.numberOfBytesToSend());
    for(int i=0; i<numberOfGateways; i++) {
        gateways[i]->sendNetworkMessage(topicMessage);
    }
}


void TopicReporter::sendToGateway(TopicListReport& list, Gateway* gateway) {
    prepareNetworkMessage(topicMessage,0,&list,list.numberOfBytesToSend());
    gateway->sendNetworkMessage(topicMessage);
}


void TopicReporter::addGateway(Gateway* gateway) {
    gateways[numberOfGateways]= gateway;
    numberOfGateways++;

    if(numberOfGateways > MAX_NUMBER_OF_GATEWAYS_PER_ROUTER) {
        ERROR("Too many Gateway added to a TopicReporter\n");
        numberOfGateways--;
    }
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
