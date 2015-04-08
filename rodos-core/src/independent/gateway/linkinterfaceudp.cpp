

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


#include"gateway/linkinterfaceudp.h"
#include "hw_specific.h"

#include "gateway/gateway.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * @file linkinterfaceudp-broadcast.cc
 * @date 2012/09/01, 2009/05/18, 2012
 * @author Sergio Montenegro, uni Würzburg
 *
 * Link Interface to broadcast networks simulated using UDP messages.
 * incomming messages will be distributed asyncrhonously using the middleware.
 *
 */


/****************************************************/

LinkinterfaceUDP::LinkinterfaceUDP(UDPInOut* udpInOut, long identifier) : Linkinterface(identifier), Subscriber(udpAsyncTopic),
    udpAsyncTopic(identifier,"udp async topic") {
    udpFromNetwork = &(udpInOut->udpIn);
    udpToNetwork = &(udpInOut->udpOut);
    isBroadcastLink = udpInOut->isBroadcastLink;
    newMessage=false;
}


void LinkinterfaceUDP::init() {
    Linkinterface::init();
    // WARNING: here we shall get a unique node number, we can not now
    // we just take the defaoult from hwInt
    udpFromNetwork->setAsync(&udpAsyncTopic); // Distribute incomming messages asyncromous to topic defaultga....
}

/************* Connection to the network, network and protocol dependent ***/

bool LinkinterfaceUDP::sendNetworkMsg(NetworkMessage &outMsg)	{
    return udpToNetwork->send(&outMsg, outMsg.numberOfBytesToSend());
}

void LinkinterfaceUDP::putFromInterrupt(const long topicId, const void* any, int len) {
    GenericMsgRef* msg = (GenericMsgRef*)any;
    incoming.put(*((NetworkMessage*)msg->msgPtr));
    if(threadToResume) threadToResume->resume();
}


bool LinkinterfaceUDP::getNetworkMsg(NetworkMessage &inMsg,int32_t &numberOfReceivedBytes) {
    numberOfReceivedBytes = -1;
    return incoming.get(inMsg);
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
