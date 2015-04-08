

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

#include "gateway/networkmessage.h"
#include "thread.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * @file linkinterface.h
 * @date 2009/05/01 7:07
 * @author Sergio Montenegro
 *
 * Copyright 2009 DLR
 *
 * @brief subscribers as gateway
 *
 */

/**
 * Interface class providing methods to connect to networks or hardware
 * interfaces and to enable data transfer.
 * The implementation is network dependent and MUST be done in derived
 * classes specific to the platform.
 */
class Linkinterface  {

    static int linkidentifierCounter;

protected:

    /// Gateway which has to be connected to the network / interface.
    const long linkIdentifier;
    Thread* threadToResume;

public:
    bool isBroadcastLink;

    /**
     * @brief	Constructor
     * @param	linkId	Unique identifier for the link within this node. 0 is
     * 					reserved for local broadcast messages, any other value
     * 					will do. Used to determine from which link a message
     * 					was received.
     */
    Linkinterface(long linkId) : linkIdentifier((linkId<0) ? linkidentifierCounter++ : linkId) {
        isBroadcastLink = false;
        threadToResume=0;
    }
    virtual ~Linkinterface() { }

    /** To initialize the hardware and to establish physical interfaces.
     * Depending on the type of interface set interrupt handlers (UART) or
     * select a port (UDP) and init the local node number if possible.
     */
    virtual void init() {}


    /**
     *Processs all received Data. When a NetworkMessage is complete it returns true
     *@param inMsg NetworkMessage to write to. When return false this may contain incomplete Data, so do not touch it
     *@param numberOfReceivedBytes When return=true this contains the number of Bytes really received or -1 if it is unknown.
     *		Should be ==Networkmessage.numberOfBytesToSend() in case of no error.
     */
    virtual bool getNetworkMsg(NetworkMessage &inMsg,int32_t &numberOfReceivedBytes)   { return false; }

    /**
     * Returns if all buffered Messages have been transmittet on the wire.
     * If this returns true the next call to sendNetworkMsg should immetiatly start sending out the new messages.
     *
     */
    virtual bool isNetworkMsgSent()                     { return true; }

    /**
     *  Sends NetworkMessage over the Link. May block. When it returns the Message should be send out or have been buffered.
     * @param The Message to send
     */
    virtual bool sendNetworkMsg(NetworkMessage& outgoingMessage) { return true;  }
    inline long getLinkdentifier() const                { return this->linkIdentifier; }

    /**
     * This Thread is resumed when there is new Data availible and getNetworkMsg should be called
     *  or when buffered Messages have been transmittet on the wire.
     */
    void setThreadToResume(Thread* thread) { this->threadToResume = thread; }
};


#ifndef NO_RODOS_NAMESPACE
}
#endif

