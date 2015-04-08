
#include "rodos.h"

static UDPInOut udp(-50000);
static LinkinterfaceUDP linkinterfaceUDP(&udp);
static Gateway gateway1(&linkinterfaceUDP, true);

// alternative:
//static Gateway gateway1(&linkinterfaceUDP);
//static TopicReporter topicReporter(&gateway1);

