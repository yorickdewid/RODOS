#include "rodos.h"

Topic<char[20]>   	collectData(-1, "CollectData");

/******************************/

class Collector : public Thread {
public:
    Collector() : Thread("Collector") { }

    void run () {
        char buf[20];
        TIME_LOOP(1*SECONDS, 2*SECONDS) {
            PRINTF("Colleting data\n");
            collectData.requestLocal(buf);
            printCharsHex("Collected:", buf, 20);
        }
    }
} collector;


class FillData : public Subscriber {
public:
    FillData() :  Subscriber(collectData, "fillData") { }
        long put(const long topicId, const long len, const void* msg, const NetMsgInfo& netMsgInfo) {
		char* toWrite = (char*)msg;
		static int cnt = 0;
		for(int i = 0; i < 20; i++) toWrite[i] = cnt*0x10 + i;
		cnt++;
		return 1;
	}
} fillData;

