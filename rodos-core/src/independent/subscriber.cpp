

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
* @file subscriber.cc
* @date 2008/09/01 7:07
* @author Sergio Montenegro, Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief topic for middleware
*
*/

// List Subscriber::subscriberList = 0;



#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

Subscriber::Subscriber(TopicInterface& topic, Putter& receiver, const char* name, bool isAGatewayParam) :
    ListElement(subscriberList),  topicInterface(topic) {
    this->isAGateway = isAGatewayParam;
    this->isEnabled = true;
    this->name = name;
    this->receiver = &receiver;
}

Subscriber::Subscriber(TopicInterface& topic, const char* name) :
    ListElement(subscriberList),  topicInterface(topic) {
    this->isAGateway = false;
    this->isEnabled = true;
    this->name = name;
    this->receiver = 0;
}


//bool Subscriber::wants(const long topicId) const {
//	return (isEnabled && (topicId == topicInterface.topicId));
//}

void Subscriber::enable(bool onOff) { isEnabled = onOff; }

bool Subscriber::isGateway() const { return isAGateway; }


/**
 * Forward the message to the Subscriber owning receiver: a putter
 */

//TODO: Remove this after 2013
long Subscriber::put(const long topicId, const long len, const void* data, long linkId) {
    return 123456789; // Retrun this magic number to see if this methode was overriden by a subclass
}

long Subscriber::put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) {
    if(!isEnabled) return 0;
    protector.enter();
    if(receiver) {
        receiver->putGeneric(topicId, len,data, netMsgInfo);
    } else {
        //TODO: Remove this after 2013
        if(put(topicId,len,data,netMsgInfo.linkId)!= 123456789) { //Print ERROR when the deprecated method is overriden
            ERROR("WARNING: If you see this message, you are overriding a deprecated put function");
            return 1;
        }
    }
    protector.leave();
    return receiver? 1 : 0;
}


void Subscriber::putFromInterrupt(const long topicId, const void* any, int len) {
    if(receiver) {
        NetMsgInfo dummy;
        receiver->putGeneric(topicId, len, any, dummy);
    }
}


#ifndef NO_RODOS_NAMESPACE
}
#endif


