

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



#include "rodos.h"

/**
 * @file topic.cc
 * @date 2008/09/01 7:07
 * @author Sergio Montenegro, Lutz Dittrich
 *
 * Copyright 2008 DLR
 *
 * @brief topic for middleware
 *
 */

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/*************************************************/

// List TopicInterface::topicList = 0; This shall be here, but moved to main

static Application applicationName("Topics & Middleware", APID_MIDDLEWARE);


TopicInterface::TopicInterface(long id, long len, const char* name) : ListElement(topicList, name)  {
    mySubscribers = 0;
    topicId = id;
    msgLen = len;
    topicFilter = 0;

    if(topicId == -1) {
        topicId = hash(name) ;
        if(topicId < FIRST_USER_TOPIC_ID) { // reserved topic ids
            topicId +=  FIRST_USER_TOPIC_ID;
        }
    }

    /** Check for replications **/
    ITERATE_LIST(TopicInterface, topicList) {
        if((iter->topicId == id) && (iter != this)) {
            ERROR("Duplicated topicId");
            PRINTF("Duplicated topicId %ld, name1 = %s name2 = %s\n", id, name, iter->name);
        }
    }

}

void TopicInterface::setTopicFilter(TopicFilter* filter) {
    if(topicFilter != 0) {
        ERROR("More than one topicFilter for topic");
        PRINTF(" topic %s has more than one filter\n", name);
    }
    topicFilter = filter;
}

TopicInterface*  TopicInterface::findTopicId(long watedTopicId) {
    ITERATE_LIST(TopicInterface, topicList) {
        if(iter->topicId == watedTopicId)  return iter;
    }
    return 0;
}


/**********************/

unsigned long TopicInterface::publish(void* data, bool shallSendToNetwork, NetMsgInfo* netMsgInfo) {
    return publishMsgPart(data,msgLen,shallSendToNetwork,netMsgInfo);

}

unsigned long TopicInterface::publishMsgPart(void* data, unsigned int lenToSend, bool shallSendToNetwork, NetMsgInfo* netMsgInfo) {
    int cnt = 0; // number of receivers a message is sent to
    NetMsgInfo localmsgInfo;

    if(!netMsgInfo) {
        localmsgInfo.linkId=RODOS_LOCAL_BROADCAST;
        localmsgInfo.sentTime     = NOW();
        localmsgInfo.senderNode   = getNodeNumber();
        localmsgInfo.senderThreadId=(uint32_t)Thread::getCurrentThread();
        netMsgInfo= & localmsgInfo;
    }


    /** If a filter is installed, it may modify the msg bevor the subscriver tet it **/
   if(topicFilter != 0)  topicFilter->modify(topicId, lenToSend, data, *netMsgInfo);

    /** Distribute to all (and only) my subscribers **/
    ITERATE_LIST(Subscriber, mySubscribers) {
        cnt += iter->put(topicId, lenToSend, data, *netMsgInfo);
    }

    if(!shallSendToNetwork) { return cnt; }

    /** Now distribute message to all gateways **/
    ITERATE_LIST(Subscriber, defaultGatewayTopic.mySubscribers) {
        cnt += iter->put(topicId, lenToSend, data, *netMsgInfo);
    }
    return cnt;
}

void TopicInterface::publishFromInterrupt(void *any, int len) {
    ITERATE_LIST(Subscriber, mySubscribers) {
        iter->putFromInterrupt(topicId, any, len);
    }
}



// Basic topic used for TBA
Topic<GenericMsgRef> defaultGatewayTopic(0, "gatewayTopic");
Topic<NetworkMessage> defaultRouterTopic(-1, "routerTopic");

Topic<void*> interruptTimer(-1,   "TimerInterrupt");
Topic<void*> interruptUart(-1,    "UartInterrupt");
Topic<void*> interruptSigterm(-1, "SigTermInterrupt");
Topic<GenericMsgRef> charInput(-1, "CharInput");


#ifndef NO_RODOS_NAMESPACE
}
#endif

