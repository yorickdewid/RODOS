

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
 * @file topic.h
 * @date 2008/09/01 7:07
 * @author Sergio Montenegro
 *
 * Copyright 2008 DLR
 *
 * @brief topic for middleware
 *
 */

#pragma once

#include "debug.h"
#include "gateway/networkmessage.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class NetMsgInfo;
class TopicFilter;

#define	RODOS_LOCAL_BROADCAST	0

/**
 *  @class TopicInterface
 *  @brief TopicInterface only for internal use
 *
 *  The middleware communication is based on the publisher/subscriber protocol.
 *  Publishers make messages public under a given topic.
 *  Subscriber to a given topic get all messages which are published
 *  under the given topic.
 * To establish a transfer path, both,
 * publisher and subscriber have to share the same topic.
 *
 * A Topic is a pair: data-type and an integer representing
 * a topic identifier.
 *
 * To access topics in an generic way which is independent of
 * data type and topicId, a TopicInterface is created.
 *
 */

class TopicInterface : public ListElement {

	friend void initSystem();
	friend class Gateway;
	friend class Subscriber;
	// friend class NetworkPoller; // TODO remove this
	friend class TopicReporter;

public:


//protected:
	static List topicList; ///< List of all topics present in the system
	List mySubscribers; ///< List of pointers to subscribers associated to one topic instance
        TopicFilter* topicFilter; ///< a filter may modify the content of the message befor the subscriver get it.
        
	long topicId; ///< Topic ID used for identification by network tramsmitions
	long msgLen; ///< Size of message transfered via this topic



public:

    TopicInterface(long id, long len, const char* name);

    virtual ~TopicInterface() { ERROR("Topic Deleted"); }

    /** publish/distribute the message to all listeners to the given serviceId.
     * if shallSendToNetwork the message will be forwarded to gateways too (default).
     * warning 1: Never use it from an interrupt server.
     * warning 2: the pointer to msg will be distributed. A Subscriber may modify its content
     */
    unsigned long publish(void *msg, bool shallSendToNetwork = true,
    		NetMsgInfo* netMsgInfo = 0);

    /** Usually we use the length of the corresponding data type associated to the topic
     * but for some exceptional cases, like for strings, we may send shorter messages.
     * warning 1: Never use it from an interrupt server.
     * warning 2: the pointer to msg will be distributed. A Subscriber may modify its content
     */
    unsigned long publishMsgPart(void *msg, unsigned int lenToSend,
            bool shallSendToNetwork = true, NetMsgInfo* netMsgInfo = 0);

    /** Publishfrom interrupts uses no semaphores as protection!
      * the Subscriber shall use no thread operations
      * the Subscriber shall be as short as possible
      */
    void publishFromInterrupt(void *any, int len = 0);

    /** request is like publish, but the calles expects data back instead of sending it.
      * The internal mechanims is identical the same like publisch, the middleware will
      * call puuters from subscribers. Such puters shall proivde data instad of geting it.
      **/

     inline unsigned long requestLocal(void *msg) { return publish(msg, false); }

     /// return 0 it not found
     static TopicInterface* findTopicId(long wantedTopicId);

     void setTopicFilter(TopicFilter* filter);

};


/**
 *  @class Topic
 *  @brief Topic  to patch publisher and subscribers
 *
 *  The middleware communication is based on the publisher/subscriber protocol.
 *  Publishers make messages public under a given topic.
 *  Subscriber to a given topic get all messages which are published
 *  under the given topic.
 * To establish a transfer path, both,
 * publisher and subscriber have to share the same topic.
 *
 * A Topic consists of a pair: a data-type and an integer representing
 * the topic identifier.
 *
 * Topic provides methods to publish and distribute messages.
 *
 * The current implementation accepts only shorts as topicId
 * because of limitations on the FPGA (Hardware) Middleware
 * implementation.
 *
 * topic id 0 is reserved for middleware messages (eg. topic lists)
 * if you use topic id -1 the system will generate an id according
 * to the name used in the constructor. This number will be
 * compatible with all hardware implementations. We recommend
 * to use this feature. This is compatible with our hardware
 * networks.
 *
 */

/*************************************/

template <class Type>
class Topic : public TopicInterface {
public:

    /** constructor, parameter name is used for debug purposes and as hash value
      * to generate a topic id if it was defined as -1. This is the proposed
       * method.
      */
    Topic(long id, const char* name) : TopicInterface(id, sizeof(Type), name) { }

    ~Topic() {
        ERROR("topic deleted");
    }

    /** publish/distribute the message to all listeners to the given serviceId.
     * if shallSendToNetwork the message will be fordarded to gateways too (default)
     * warning 1: Never use it from an interrupt server.
     * warning 2: the pointer to msg will be distributed. A Subscriber may modify its content
     */
    inline unsigned long publish(Type &msg, bool shallSendToNetwork = true) {
        return TopicInterface::publish(&msg, shallSendToNetwork);
    }

    /** To publish constants,
     * But please only for basic data types (char, short, long, float, double)
     */
    inline unsigned long publishConst(Type msgConst, bool shallSendToNetwork = true) {
        Type msg = msgConst;
        return TopicInterface::publish(&msg, shallSendToNetwork);
    }


    /** To send only a part of the data type, and not the whole object.
      *for example for strings
      * warning: Never us it from an interrupt server.
      */
    inline unsigned long publishMsgPart(Type &msg, unsigned int lenToSend, bool shallSendToNetwork = true) {
        return TopicInterface::publishMsgPart(&msg, lenToSend, shallSendToNetwork);
    }

    /** request is like publish, but the calles expects data back instead of sending it.
      * The internal mechanims is identical the same like publisch, the middleware will
      * call puuters from subscribers. Such puters shall proivde data instad of geting it.
      **/

     inline unsigned long requestLocal(Type &msg) { return TopicInterface::publish(&msg, false); }

};

/***
 ** very often we need a generiv type for topics which contians just
 ** a pointer and a len.
 ** to avoid 100x declarations fo this struct, we define ohe here
 **/

struct GenericMsgRef {
    long  context; ///< any further infoy you would like to deliver
    char* msgPtr;
    int   msgLen;

    GenericMsgRef() { msgLen = context = 0; msgPtr = 0; }
};

/**************************************************************/
/** A topic only for gateways */

extern Topic<GenericMsgRef> defaultGatewayTopic; //All localy publiched Topics go here for the Gateways and/or Routers to send out
extern Topic<NetworkMessage> defaultRouterTopic; //All incoming Messages are publiched here by the Gateways.
/************ Predifined Topics from Interrupt servers **********/

extern Topic<void*> interruptTimer;
extern Topic<void*> interruptUart;
extern Topic<void*> interruptSigterm;
extern Topic<GenericMsgRef> charInput; ///< used instead of getcharNoWait()


#ifndef NO_RODOS_NAMESPACE
}
#endif

