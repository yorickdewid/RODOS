#include "rodos.h"
#include "mytopics.h"


/******************************/

static Application senderName("Publisher ping", 2011);

int64_t tSend, tReceive;
class PingSender : public Thread, Subscriber {
public:
   PingSender(): Thread("ppingsender"), Subscriber (pong, "pongReceiver") { }
   void run () {
	long pingCnt = 0;
	TIME_LOOP(0, 1*SECONDS) {
	  PRINTF("ping %ld at %lld\n", pingCnt, NOW());
          tSend = NOW();
	  ping.publish(pingCnt);
          pingCnt++;
	}
   }

   long put(const long topicId, const long len, const void* data,  const NetMsgInfo& netMsgInfo) {
        tReceive = NOW();
        long cnt = *(long*)data;
         PRINTF("pong %ld at %lld, dif = %6.3f us\n", cnt, tReceive, (double)(tReceive - tSend)/MICROSECONDS);
         return 1;
   }

} pingSender;
