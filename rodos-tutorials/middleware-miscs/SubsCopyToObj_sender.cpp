#include "rodos.h"
#include "SubsCopyToObj_topics.h"

static Position myPos;

class MyPublisher04 : public Thread {
public:
    MyPublisher04() : Thread("SenderPosition", 150) { }
    void run () {
        int32_t cnt = 100;
        TIME_LOOP(3500*MILLISECONDS, 3*SECONDS) {
            PRINTF("Sending %d...\n", cnt);
            myPos.x = cnt++;
            myPos.y = cnt++;
            myPos.z = cnt++;
            position.publish(myPos);
        }
    }
} publisher04;

