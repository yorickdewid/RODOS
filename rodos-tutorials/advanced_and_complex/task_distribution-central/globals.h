
#ifndef __DSITRIBUTOR_GLOBALS_H__
#define __DSITRIBUTOR_GLOBALS_H__

/********************* Tasks *********************/

#define NUM_OF_TASKS		7

#define TASK_DISTRIBUTOR	1
#define TASK_TASKMANAGER	2

#define FIRST_USERTASK		3

#define TASK_NEGCNT		3
#define TASK_ODDCNT	 	4
#define TASK_EVENCNT 		5
#define TASK_PRIMCNT		6


/**********************contexts ******************/

class DistributionTable {
public:
	long nodeToExecute[NUM_OF_TASKS];
};

class NodeState {
public:
	long nodeNumber;
	TTime reportTime;
	bool nodeSane;
	long cpuLoad;
	// extentions: free mem, list of applications
};



class ContextNegCnt {
public:
    long long cnt;
};

class ContextOddCnt {
public:
    long long cnt;
};

class ContextEvenCnt {
public:
    long long cnt;
};


class ContextPrimCnt {
public:
    long long cnt;
};




/***************** Topics (topicId -1 -> automatic generated from name) *****/


extern Topic<DistributionTable> distributionTableTopic;
extern Topic<NodeState>         nodeStateTopic;

extern Topic<ContextNegCnt>  negCntTopic;
extern Topic<ContextOddCnt>  oddCntTopic;
extern Topic<ContextEvenCnt> evenCntTopic;
extern Topic<ContextPrimCnt> primCntTopic;

#endif
