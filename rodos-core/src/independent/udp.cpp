

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
 * @file udp.h
 * @date 2008/05/20 6:20
 * @author Sergio Montenegro
 *
 * Copyright 2008 DLR
 *
 * UDP interface
 *
 */


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

UDPIn::UDPIn(const long portNr) : udpRx(portNr) 			      { }
UDPIn::UDPIn(const long portNr, Topic<GenericMsgRef>* associatedTopic) : udpRx(portNr)  {udpRx.setAsync(associatedTopic); }

void UDPIn::setAsync(Topic<GenericMsgRef>* associatedTopic)             {udpRx.setAsync(associatedTopic); }
void UDPIn::reopen(const long portNr) 					{ udpRx.reopen(portNr); }

long UDPIn::get(void* userData, int maxLen) 				{ return udpRx.get(userData, maxLen); }
long UDPIn::get(void* userData, int maxLen, unsigned long *ipaddr) 	{ return udpRx.get(userData, maxLen,ipaddr); }
bool UDPIn::readyToGet() 						{ return udpRx.readyToGet(); }

bool UDPIn::isConnected() 						{ return udpRx.isInitialised(); }

/****************/

UDPOut::UDPOut(const long portNr, const char* hostname) 		: udpTx(portNr, hostname) 	{ }
//UDPOut::UDPOut(const long _portNr) 					: udpTx(_portNr) 	  	{ }
UDPOut::UDPOut(const long _portNr, unsigned long _ipAddr) 		: udpTx(_portNr, _ipAddr)	{ }
UDPOut::UDPOut(const long _portNr, int ip0, int ip1, int ip2, int ip3)	: udpTx(_portNr, ip0, ip1, ip2, ip3) { }

bool UDPOut::send(const void* userData, const int maxLen) 	        	 { return udpTx.send(userData, maxLen); }
bool UDPOut::sendTo(const void* userData, const int maxLen, unsigned long ipAddr) { return udpTx.sendTo(userData, maxLen,ipAddr); }
bool UDPOut::isConnected() 						{ return udpTx.isInitialised(); }


UDPInOut::UDPInOut(int broadcastPort):
    udpIn(broadcastPort),
    udpOut(broadcastPort) {
    isBroadcastLink=true;
    if(broadcastPort >=0) {
        ERROR("Broadcast UDPInOut defined but port number ist positive");
    }
}

UDPInOut::UDPInOut(int port ,const char* hostname):
    udpIn(port),
    udpOut(port,hostname) {
    isBroadcastLink=false;
    if(port <=0) {
        ERROR("Non-Broadcast UDPInOut defined but port number ist negative");
    }
}

UDPInOut::UDPInOut(int localInputPort, int remoteOutputPort,const  char* hostname):
    udpIn(localInputPort),
    udpOut(remoteOutputPort,hostname) {
    isBroadcastLink=false;
    if(localInputPort <=0 || remoteOutputPort <=0) {
        ERROR("Non-Broadcast UDPInOut defined but port number ist negative");
    }
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

