#include "rodos.h"
#include "../topics.h"
#include "mask.h"

/***********************************************/

static Application      senderName("timeSyncServerApp", 1000);
static TimeSyncResponse response;
static int32_t          requestCount = 0;

double timeAtLastInterrupt = 0;
/**
 * Implements a simple SNTP server. The server can process requests from multiple clients.
 *  <p>The clients send their current request count. The server fills the <pre>clientNodeNr</pre>
 * field in the <pre>TimeSyncResponse</pre> and posts the response immediatly.
 *
 * @author Chavdar Iliev (uni Würzburg)
 */
class TimeSyncServer: public Subscriber, public Putter {
public:
    TimeSyncServer() : Subscriber(clientRequest, "timeSyncServer") {}

    long put(const long _topicId, const long _len, const void* _msg, const NetMsgInfo& _netMsgInfo) {
        int32_t requestId = *(int32_t*) _msg;
        response.ntspTimes.rcvTS  = sysTime.getUTC(); // by real time server, set by hardware at arrival
        response.clientRequestCnt = requestId;
        response.clientNodeNr     = _netMsgInfo.senderNode;

        PRINTF(CL_NODE "%d" REQ "%d", response.clientNodeNr, ++requestCount);
        PRINTF(INT_TIME_SRV "%9.6f", timeAtLastInterrupt);

        response.ntspTimes.sendTS = sysTime.getUTC(); // by real time server, set by hardware at deliver time
        serverResponse.publish(response);
        return 1;
    }

    /** Just for debug: Print the utc time as response to the (global) interrupt **/
    bool putGeneric(const long _topicId, const unsigned int _msgLen, const void* _msg, const NetMsgInfo& _netMsgInfo) {
        timeAtLastInterrupt = utcMillisecsNow();
        return true;
    }
} timeSyncServer;

/// Intterupt
static Subscriber nameNotImportant(interruptSigterm, timeSyncServer, "resumeFromInterruptServer");

