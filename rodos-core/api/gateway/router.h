/*
 *
 * @file router.h
 *  Created on: 27.03.2013
 *      Author: Dilger
 */


#pragma once

#define MAX_NUMBER_OF_GATEWAYS_PER_ROUTER 8

#include "rodos.h"
#include "gateway/gateway.h"
#include "subscriber.h"
#include "putter.h"
#include "topic.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class Router : public Subscriber,Putter  {
    Subscriber localTopics;
    NetworkMessage localMessage;

protected:
    Gateway* gateways[MAX_NUMBER_OF_GATEWAYS_PER_ROUTER];
    int numberOfGateways;
    bool forwardTopicReports;

public:
    Router(bool forwardTopicReports_ = false, Gateway* gateway1=0, Gateway* gateway2=0, Gateway* gateway3=0, Gateway* gateway4=0); 
    virtual ~Router() {}

    /*
     * Networkmessages from all Gateway come in here
     */
    long put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo);

    /*
     * Local Topics to send out come in here
     */
    bool putGeneric(const long topicId, const unsigned int len, const void* msg, const NetMsgInfo& netMsgInfo);


    /**
     * Gets called for every packet that enters this Router from local or from network.
     * Default implementation make routing descision with below functions for every Gateway added to this router
     */
    virtual void routeMsg(NetworkMessage &msg, long linkid);


    /*
     * Should this Message be routet in general?
     *
     */
    virtual bool shouldRouteThisMsg(NetworkMessage &msg, long linkid);

    /**
     * Shoould this message be routet to this Gateway?
     */
    virtual bool shouldRouteThisMsgToGateway(NetworkMessage &msg, long linkid,Gateway* gateway);

    virtual void addGateway(Gateway* gateway);
};


#ifndef NO_RODOS_NAMESPACE
}
#endif

