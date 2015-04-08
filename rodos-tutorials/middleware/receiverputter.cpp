#include "rodos.h"
#include "demo_topics.h"
#include "mask.h"

static Application  receiverName("ReciverPutter", 1100);

class JustPrint : public Putter {

	/// Implements the complete generic interface of putter
        bool putGeneric(const long topicId, const unsigned int msgLen, const void* msg, const NetMsgInfo& netMsgInfo) {
		PRINTF(NB "%d" MD "%ld" TID "%ld\n", msgLen, *(long*)msg, topicId);
		return true;
	}
} justPrint;

/** Two subscriber doint the same.... */
static Subscriber nameNotImportant01(counter1, justPrint, "justprint01");
static Subscriber nameNotImportant02(counter2, justPrint, "justprint02");
//static Subscriber nameNotImportant03(position, justPrint, "justprint03");
