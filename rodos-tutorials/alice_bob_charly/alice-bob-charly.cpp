
#include "rodos.h"

/**************************************************** Common Data ***/

struct Greetings {
    int64_t date;
    char msg[80];
};

Topic <Greetings> valentine(20, "valentine");

#define VALENTINE_DAY	2*SECONDS
#define VALENTINE_DAY2	4*SECONDS

/********************************************************* Charly  ***/

class Charly : public TopicFilter {
public:
    void modify(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) {
        Greetings* grts = (Greetings*)data;
        grts->msg[0] = 'I'; // correction 
    }
} charly;


/************************************************************ Bob ***/

class Bob : public Thread {
    Greetings greetings;
public:
    void run() {
        AT(VALENTINE_DAY);
        greetings.date = NOW();
        strcpy(greetings.msg, "i wish you would be here");
        PRINTF("Bob sending greetings at %lld '%s'\n", NOW(), greetings.msg);
        valentine.publish(greetings);

        AT(VALENTINE_DAY2);
        valentine.setTopicFilter(&charly);
        valentine.publish(greetings);
    }
} bob;

/********************************************************* Alice  ***/

class Alice : public SubscriberReceiver<Greetings> {
public:
    Alice() : SubscriberReceiver<Greetings>(valentine, "aliceSubs") { }
    void put(Greetings &data) {
        Greetings* greetingsForMe = (Greetings*)&data;
        PRINTF("Alice got date %lld:  msg: %s\n", greetingsForMe->date, greetingsForMe->msg);
    }
} alice;

