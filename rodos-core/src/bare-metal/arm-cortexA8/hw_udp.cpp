/*
 *
 */
#include "rodos.h"
#include "hw_udp.h"

/*
 * UDPReceiver class
 */
UDPReceiver::UDPReceiver(const TUDPPortNr port){}

UDPReceiver::~UDPReceiver(){}

void UDPReceiver::reopen(const TUDPPortNr port){}

void UDPReceiver::setAsync(Topic<GenericMsgRef>* associatedTopic){}


long UDPReceiver::get(void* userData, const unsigned int maxLen){
	return 0;
}

long UDPReceiver::get(void* userData, int maxLen, unsigned long *ipaddr){
	return 0;
}

bool UDPReceiver::readyToGet() {
	return false;
}

/*
 * UDPTransmitter class
 */
UDPTransmitter::UDPTransmitter(const TUDPPortNr port,  const char *host){}

UDPTransmitter::UDPTransmitter(const long _portNr, unsigned long _ipAddr){}

UDPTransmitter::UDPTransmitter(const long _portNr, int ip0, int ip1, int ip2, int ip3){}

UDPTransmitter::~UDPTransmitter(){}

bool UDPTransmitter::send(const void*msg, const unsigned int len){
	return false;
}

bool UDPTransmitter::sendTo(const void* userData, const int maxLen, unsigned long _ipAddr){
	return false;
}

