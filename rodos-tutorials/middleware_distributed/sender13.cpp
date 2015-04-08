#include "rodos.h"
#include "demo_topics.h"
#include "mask.h"

/******************************/

static Application senderName("Publisher 13", 2013);

class MyPublisher13 : public Thread {
public:
    MyPublisher13() : Thread("sender13") { }
    void run () {
        long cnt = 10000;
        TIME_LOOP(0,  1600*MILLISECONDS) {
            PRINTF("%s%ld", S13, ++cnt);
            counter3.publish(cnt);
        }
    }
} myPublisher13;

