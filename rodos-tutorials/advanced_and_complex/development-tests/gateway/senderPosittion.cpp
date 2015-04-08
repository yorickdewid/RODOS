#include "rodos.h"

#include "demo_topics.h"


/******************************/

static Application senderName("Publisher 03 Position", 300);

static Position myPos;

class MyPublisher03 : public Thread {

public:
	MyPublisher03() : Thread("SenderPosition") { }

	void run () {
		long cnt = 10000;
		TIME_LOOP(3*SECONDS, 3*SECONDS) {
			PRINTF("SenderPos -> %ld\n", cnt);
			myPos.x = cnt++;
			myPos.y = cnt++;
			myPos.z = cnt++;
			position.publish(myPos);
		}
	}
};

static MyPublisher03 myPublisher;

