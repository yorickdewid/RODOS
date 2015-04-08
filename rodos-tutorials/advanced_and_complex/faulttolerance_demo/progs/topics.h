
#include "rodos.h"
/** Topics ***/

struct Coordinates {
	long msgCnt;
	long subCounter;
	long lastX, lastY;
	long x, y;
	long myNode;

	Coordinates() { x = y = lastX = lastY = subCounter = msgCnt = myNode = 0; }
	void printIt() {
		PRINTF("@%ld: %ld.%ld (%ld, %ld) -> (%ld, %ld)\n", myNode, msgCnt, subCounter, lastX, lastY, x, y);
	}
};

extern Topic<Coordinates>  fromSensor;
extern Topic<Coordinates>  toActuator;
extern Topic<long>		   killNode;


#define MY_NODE_NR() ( getNodeNumber() % 10 )
