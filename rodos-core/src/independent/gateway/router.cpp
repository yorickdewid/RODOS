

/*********************************************************** Copyright
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/


/**
 * @file router.cpp
 * @date 2013/03/27 17:46
 * @author Erik Dilger, Uni W�rzburg
 *
 * Copyright 2009 DLR
 *
 * @brief gateway for middleware
 *
 */

#include "gateway/router.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

#define not0(a) ((a)?1:0)

Router::Router(bool forwardTopicReports_, Gateway* gateway1, Gateway* gateway2, Gateway* gateway3, Gateway* gateway4) :
    Subscriber(defaultRouterTopic,"Router"),
    localTopics(defaultGatewayTopic,*this,"Router") {

    forwardTopicReports=forwardTopicReports_;
    gateways[0]      = gateway1;
    gateways[1]      = gateway2;
    gateways[2]      = gateway3;
    gateways[3]      = gateway4;
    numberOfGateways = not0(gateway1) + not0(gateway2) + not0(gateway3) + not0(gateway4);
}


long Router::put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) {
    routeMsg(*((NetworkMessage*)data),netMsgInfo.linkId);
    return 1;
}

bool Router::putGeneric(const long topicId, const unsigned int len,
                        const void* msg, const NetMsgInfo& netMsgInfo) {

    protector.enter();
    prepareNetworkMessage(localMessage,topicId,msg,len);
    routeMsg(localMessage,RODOS_LOCAL_BROADCAST);
    protector.leave();

    return true;
}

void Router::routeMsg(NetworkMessage& msg,long linkid) {
    if(shouldRouteThisMsg(msg,linkid)) {
        msg.setCheckSum();

        for(int i=0; i<numberOfGateways; i++) {
            if(shouldRouteThisMsgToGateway(msg,linkid,gateways[i])) {
                gateways[i]->sendNetworkMessage(msg);
            }
        }
    }
}

bool Router::shouldRouteThisMsg(NetworkMessage& msg,long linkid) {
    if(msg.get_maxStepsToForward() <= 0)               return false;
    if(msg.get_topicId() == 0 && !forwardTopicReports) return false;
    return true;
}

bool Router::shouldRouteThisMsgToGateway(NetworkMessage& msg,long linkid, Gateway* gateway) {
    if(gateway->getLinkIdentifier()==linkid) return false;
    return gateway->shouldTopicForwarded(msg.get_topicId());
}

void Router::addGateway(Gateway* gateway) {
    gateways[numberOfGateways]= gateway;
    numberOfGateways++;

    if(numberOfGateways > MAX_NUMBER_OF_GATEWAYS_PER_ROUTER) {
        ERROR("Too many Gateway added to a router\n");
        numberOfGateways--;
    }
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
