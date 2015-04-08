#include "rodos.h"
#include "demo_topics.h"

static Application  receiverName("ReciverPutter", 1100);

class JustPrint : public Putter {

	/// Implements the generic interface von putter
	bool putGeneric(unsigned int msgLen, const void* msg) {
		PRINTF("			Pututer gets %d bytes = %ld\n", msgLen, *(long*)msg);
		return true;
	}
};

JustPrint justPrint;

/** Two subscriber doint the same.... */
static Subscriber nameNotImportant01(counter1, justPrint);
static Subscriber nameNotImportant02(counter2, justPrint);
