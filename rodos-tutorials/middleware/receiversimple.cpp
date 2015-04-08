#include "rodos.h"
#include "demo_topics.h"
#include "mask.h"

static Application  receiverName("ReciverSimple", 1100);

/*********** Example 1 *************/
class SimpleSub :  public Subscriber {
public:
	SimpleSub() : Subscriber(counter1, "simplesub") { }
        long put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) {
		PRINTF(E1B "%ld" E1D "%ld" F1 "%ld" F2 "\n", len, *(long*)data, topicId);
		return 1;
	}
} simpleSub;

/*********** Example 2 *************/

class SimpleSub2 : public SubscriberReceiver<long> {
public:
    SimpleSub2() : SubscriberReceiver<long>(counter1, "sb2") { }
    void put(long &data) {
        PRINTF(E2D "%ld\n", data);
    }       
}simpleSub2;


/*********** Example 3 *************/

void printMsg(long& val) {
    PRINTF( E3D "%ld\n", val);
}
SubscriberReceiver<long> naneNotImportant02(counter1, printMsg, "readerfunc");

