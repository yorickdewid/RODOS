

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
* @file subscriber.h
* @date 2008/09/01 7:07
* @author Sergio Montenegro
*
* Copyright 2008 DLR
*
* @brief subscribers for middleware
*
*/

#pragma once

#include "semaphore.h"
#include "topic.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class Putter;

/**
* @class Subscriber
* @brief Subscriber to receive messages
*
*  The middleware communication is based on the publisher/subscriber protocol.
*  Publishers make messages public under a given topic.
*  Subscribers to a given topic get all messages which are published
*  under the given topic.
*  To establish a transfer path, both:
*  publisher and subscriber have to share the same topic.
*
* Each subscriber has a reference to the associated topic and a putter
* to store messages.
* All subscribers are registered in a list.
* Each time a messages is published the list of all subscriber will be search and
* for each subscriber where the topic matches the associated putter will be called
* to store a copy of the published message.
*/

class NetMsgInfo {
public:
    int32_t senderNode;	///< Node ID of sending instance of RODOS
    int64_t sentTime;   ///< Time in localTime units 
    uint32_t senderThreadId;
    uint32_t linkId;
};


class Subscriber : public ListElement {

    friend class TopicInterface;
    friend class TopicReporter;
    friend void initSystem();

public:
    /// Reference to a topic to which the subscriber listens.
    TopicInterface& topicInterface;

    /// Reference to the associated handler of a message received.
    Putter* receiver;

protected:
    /// Flag to indicate if the subscriber broadcasts a message internally or externally
    bool isAGateway;
    /// Flag to activate or deactivate the subscriber
    bool isEnabled;
    /// List of all subscribers in the system
    static List subscriberList;

    Semaphore protector;
    /// DEPRECATED! DO not use anymore!
    virtual long put(const long topicId, const long len, const void* data, long linkId);
    /// Default function: forward the message and invoke the receiver (putter). It locks the semaphore protector
    virtual long put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo);

    /// do not locks any semaphore. Do not call any thread funcion
    /// default function resumes the associated thread (if defiend) if it is waiting for it
    virtual void putFromInterrupt(const long topicId, const void* any, int len = 0);


public:

    /** At creation time a connection to the corresponding topic
     * and putter (receiver) has to be done.
     * Subscriber my be "Normal" message receiver or may be Gateways.
     * Gatewayes are interfaces to other memory spaces and nodes, e.g.
     * going throug a network. Gateways just forward messages to other nodes.
     *
     * The parameter name is only used for debug purposes, to see which subscriber is
     * attached to which topic. Using names you can recognise present subscribers in listings.
     *
     * @param topic reference to the topic to which the owner will be subscribed
     * @param putter reference to a putter receiving messages from the topic subscribed
     * @param name name is used only for debug purposes, sing it one can recognise Threads.
     * @param isAGatewayParam : gateways get from all topics and they decide to forward or not
     */
    Subscriber(TopicInterface &topic, Putter& receiver, const char* name = "anonymSubscriber", bool isAGatewayParam=false);

    /**
      * Simple subscriber has no ssociated receiver (nor thread nor putter), they just (have to)
      * redifine the put method
      */
    Subscriber(TopicInterface &topic, const char* name = "anonymThreadSubscriber");

    /// Destructor, should not be called in nominal operation.
    virtual ~Subscriber() { ERROR("Subscriber deleted"); }

    /** returns true if InputPort is enabled and wants to receive messages for
    *   given topicId
    */
    // virtual bool wants(const long topicId) const;

    /**
     * @return enabled == true -> receive all messages to the given ServiceId,
    *    else ignore them
    */
    void enable(bool onOff = true);

    /** Check whether the current subscriber is a gateway,
     * hence connects to the network.
     * @return if true -> message is sent to the network, false -> is send locally
     */
    bool isGateway() const;

};


/** Simple Subscriver interface for userd ************/

template <class Type>
class SubscriberReceiver : public Subscriber {
    void (*receiverFunc)(Type &msg); // a functio to be called for each message (optional)

public:
    SubscriberReceiver(TopicInterface &topic, const char* name = "anonymSubscriber") :
        Subscriber(topic, name) {
        receiverFunc = 0;
    }
    SubscriberReceiver(TopicInterface &topic,  void (*funcPtr)(Type&), const char* name = "anonymSubscriber") :
        Subscriber(topic, name) {
        receiverFunc = funcPtr;
    }

    virtual void put(Type &msg) {if(receiverFunc) (*receiverFunc)(msg); };
    virtual void put(Type &msg, const NetMsgInfo& netMsgInfo) {put(msg);};
    long put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) { 
        put(*(Type*)data,netMsgInfo);
        return 1;
    }
};


/** a filter which may modify the content of published messages
 ** WARNING: Please be very carfully when using filters
 ** do not abuse of it. It is intended for voters, kalman filter, etc
 **/


class TopicFilter {
public:
    virtual void modify(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) = 0;
};



#ifndef NO_RODOS_NAMESPACE
}
#endif

