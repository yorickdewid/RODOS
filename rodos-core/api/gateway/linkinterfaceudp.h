/*
 * linkinterfaceudp-broadcast.h
 *
 *  Created on: 27.03.2013
 *      Author: erik
 */

#pragma once

#include "gateway/linkinterface.h"
#include "subscriber.h"
#include "udp.h"
#include "rodos.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class LinkinterfaceUDP : public Linkinterface,Subscriber  {

protected:
    /// UDP transmitter connecting via UDP to the network switch.
    UDPIn*  udpFromNetwork;
    UDPOut* udpToNetwork;

    long udpPortNumber;   ///< Number of input port, also distributed to the network
    long nodeNumber;      ///< node number used in network, linked with udpPortNumber

private:

    Topic<GenericMsgRef> udpAsyncTopic;
    Fifo<NetworkMessage, 1500> incoming;
    //CommBuffer<NetworkMessage> incoming;
    bool newMessage;

protected: //This is for LinkinterfaceUDP
public:

    LinkinterfaceUDP(UDPInOut* udpInOut, long identifier = -1);

    virtual ~LinkinterfaceUDP() { }
    /**
     * Link interface to the gateway, look up a free UDP input port and
     * assign a corresponding node number used as ID in the network.
     */
    void init();

    /**
     * Retrieve a message in network potocol format from an associated UDP port.
     * @param outMsg pointer to message to send wrapped in network structure
     * @return true - if sending was successful, false - otherwise
     */
    bool sendNetworkMsg(NetworkMessage &outMsg);
    bool getNetworkMsg(NetworkMessage &inMsg,int32_t &numberOfReceivedBytes);

    virtual void putFromInterrupt(const long topicId, const void* any, int len = 0);
};

#ifndef NO_RODOS_NAMESPACE
}
#endif

