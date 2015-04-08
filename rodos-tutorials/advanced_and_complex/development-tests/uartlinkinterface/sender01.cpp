#include "rodos.h"

#include "demo_topics.h"


/******************************/

static Application senderName("Publisher 01 simple", 100);

class MyPublisher01 : public Thread {

public:
    MyPublisher01() : Thread("SenderSimple") { }

    void run () {
        char outStr[10];

        long cnt = 'A' | ('B'<<8) | ('C' << 16) | ('D' << 24);

        TIME_LOOP(3*SECONDS, 5*SECONDS) {
            memcpy(outStr, &cnt, 4);
            outStr[4] = 0;
            PRINTF("Sender 01 -> %ld = %s\n", cnt, outStr);
            setCahr.publish(cnt);
            cnt++;
        }
    }
};

static MyPublisher01 myPublisher;

