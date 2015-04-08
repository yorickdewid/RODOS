

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



#pragma once

#include "gateway/linkinterface.h"
#include "subscriber.h"
#include "putter.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * @file gateway.h
 * @date 2009/05/01 7:07
 * @author Sergio Montenegro
 *
 * Copyright 2009 DLR
 *
 * @brief subscribers as gateway
 *
 */

/**
 * A gateway is a link from the local middleware to an external network.
 * A single middleware may have many gateways, each connecting to one different external link.
 * To send messages, a Gateway works like a normal subscriber to read data from the middleware topics,
 * but it can respond to more than one topic.
 * Messages received from the external link will be distributed in the
 * loccal middleware using the same mechaninsm like normal topics.
 *
 * To read from the network link, the gateway implements a thread
 * which may poll the link or may be reactivated by interrupts from the link.
 *
 * Some intelligent networks may deliver a list of topics which are expected
 * externally. To access other networks, the user has to define which
 * topics shall be forwarded (or forward all, but may be inefficient TODO OFr Explain why!).
 *
 */

struct SeenNode {
    int32_t  nodeID;
    uint32_t sequenceCounter;
    int64_t  lastMsgTime;
};


class Gateway : public Subscriber, public Thread {

private:
    Putter nopPutter; ///< inherited from parent but never used, only as placeholder
    Semaphore networkOutProtector;
    NetworkMessage networkOutMessage; // protected with semaphore
    NetworkMessage networkInMessage;

    /**
     * List of topics, which shall be forwarded to the external network.
     * For simple networks this could be a constant list.
     * For intelligent networks (e.g. using UDP-networks) this list can be provided
     * by the networks using topic-id 0.
     */
    TopicListReport externalsubscribers;

    /** If set true, forces me to forward ALL messages, otherwise only for topics listed */
    bool forwardAll;

    /** Hardware abstraction to access the link interface to the network */
    Linkinterface* linkinterface;
    long linkIdentifier;

    bool getTopicsToForwardFromOutside;


    /** Transfer messages from the local network to the external network.
     * @param[out] topicId ID of sending topic
     * @param[out] len length of message to send (size of topic type)
     * @param[out] data pointer to data field (type) of message
     * @return number of receivers message is sent to, always 1
     */
    virtual long put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo);

    void AnalyseAndDistributeMessagesFromNetwork();

    static SeenNode seenNodes[MAX_NUMBER_OF_NODES];
    static int32_t numberOfNodes;
    static Semaphore seenNodesProtector;

    static bool messageSeen(NetworkMessage& msg);

public:

    /* From sublcases */
    void run();


    /** At creation time a connection to the corresponding topic
     * and putter (receiver) has to be done.
     * Subscriber may be "Normal" message receiver or may be gateways.
     * Gateways are interfaces to other memory spaces and nodes, example
     * going through a network. Gateways just forward messages to other nodes.
     *
     * The param linkinterface is an object which shall be used to
     * access the hardware of the link interface to the network.
     * @param linkinterface_ instance of linking interface to either UDP or UART
     * @param forwardall_ flag whether to send messages for all or only selected topics
     * @param enable flag whether this gateway should send localy published topics to the network, disable if using routers to do this
     */
    Gateway(Linkinterface* linkinterface_, bool forwardall_ = false,bool enable_ = true);

    /**
      *Send out msg over this Gateways LinkInterface.
      *Send Is used by the Gateway itsself and may be used by external modules like Routers.
      */
    void sendNetworkMessage(NetworkMessage& msg);


    bool shouldTopicForwarded(long topicId) { return forwardAll ? true : externalsubscribers.find(topicId);}
    /**
     * For Simple networks, the list of topics which shall be forwarded
     * to the network has to be known in the local node. This
     * list has to be passed to the gateway using setTopicsToForward(...)
     * Intelligent networks (for example the implementation using UDP)
     * the network provides this list and the receiver thread will
     * call this method for arrived messages with topic 0 (list report).
     */

    void setTopicsToForward(void* topicList);
    void addTopicsToForward(void* topicList);
    void addTopicsToForward(TopicInterface* topicId1,TopicInterface* topicId2=0,TopicInterface* topicId3=0,TopicInterface* topicId4=0);
    void resetTopicsToForward();


    TopicListReport* getTopicsToForward() { return &externalsubscribers;}

    /**
     * In worst case you may forward all topics. Say true/false.
     * If you can not write to the topic list with setTopicsToForwarded
     * then you might use this by triggering the flag.
     */
    void setForwardAllTopics(bool forwardall = true);

    long getLinkIdentifier() {return linkinterface->getLinkdentifier();}

};

void prepareNetworkMessage(NetworkMessage& netMsg, const long topicId,const void* data, int len);


#ifndef NO_RODOS_NAMESPACE
}
#endif


