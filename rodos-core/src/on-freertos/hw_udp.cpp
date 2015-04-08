#include "rodos.h"
#include "hw_udp.h"


UDPReceiver::UDPReceiver(const TUDPPortNr port)
{
}
UDPReceiver::~UDPReceiver()
{
}
void UDPReceiver::reopen(const TUDPPortNr port)
{
}
/*
bool UDPReceiver::isConnected()
{
	if(pcbUdp == NULL)
		return false;
	else
		return true;
}
*/
bool UDPReceiver::readyToGet()
{
	return false;
}
long UDPReceiver::get(void* userData, const unsigned int maxLen)
{
	return 0;
}
long UDPReceiver::get(void* userData, int maxLen, unsigned long *ipaddr)
{
	return 0;
}

void UDPReceiver::setAsync(Topic<GenericMsgRef>* associatedTopic){
    
}

UDPTransmitter::UDPTransmitter(const TUDPPortNr port,  const char *host)
{
}
UDPTransmitter::UDPTransmitter(const long _portNr, unsigned long _ipAddr)
{
}
UDPTransmitter::UDPTransmitter(const long _portNr, int ip0, int ip1, int ip2, int ip3)
{
}
UDPTransmitter::~UDPTransmitter()
{
}
bool UDPTransmitter::send(const void*msg, const unsigned int len)
{
    	return false;
}
bool UDPTransmitter::sendTo(const void* userData, const int maxLen, unsigned long _ipAddr)
{
    	return false;
}
