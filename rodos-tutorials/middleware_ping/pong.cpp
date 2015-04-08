#include "rodos.h"
#include "mytopics.h"


/******************************/

static Application Receiver("Subscriber pong", 2011);

class PingReceiver :  public Subscriber {
public:
        PingReceiver() : Subscriber(ping, "pingReceiver") { }
        long put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) {
                long cnt = *(long*)data;
                pong.publish(cnt);
                PRINTF("got ping %ld \n", cnt);
                return 1;
        }
} pingreceiver;

