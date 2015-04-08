#include "rodos.h"

#include "demo_topics.h"

static Application senderName("Publisher 02 Timeevent", 200);

class MyPublisher02 : public TimeEvent {
public:
  void handle(){
    xprintf("sendTimed at %lld\n", NOW());
    long seconds = NOW() / SECONDS;
    counter2.publish(seconds);
  }

  void init() {
    activatePeriodic(4*SECONDS, 500*MILLISECONDS);
  }
};


static MyPublisher02 myPublisher;

