
#include "rodos.h"


static UDPInOut udp3(-50002);  
static LinkinterfaceUDP linkinterfaceUDP3(&udp3);
static Gateway gateway3(&linkinterfaceUDP3, true);

