#include "rodos.h"
#include "demo_topics.h"
#include "mask.h"

/******************************/

static Application senderName("Publisher 16", 2016);

class MyPublisher16 : public Thread {
public:
    MyPublisher16() : Thread("sender16") { }
    void run () {
        long cnt = -20000;
        TIME_LOOP(0, 2600*MILLISECONDS) {
            PRINTF("%s%ld", S16, --cnt);
            counter6.publish(cnt);
        }
    }
} myPublisher16;

