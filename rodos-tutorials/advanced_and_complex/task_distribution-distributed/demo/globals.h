
#ifndef __GLOBALS_H__
#define __GLOBALS_H__


/*** Support for context distribution ****/

struct Context {
    long long contextCnt;
    int nodeNr;
    long value;
    char controllerId;
    Context() { contextCnt = 0; value = 0; controllerId = '-'; nodeNr = -1; }
};

struct NodeState {
    int nodeNr;
    TTime reportTime;
    int cpuLoad;
};

extern Semaphore loadProtector;
extern int minLoadGlobal;
extern int myLoadGlobal;

extern Topic <Context> topicX;
extern Topic <Context> topicY;
extern Topic <Context> topicZ;

extern Topic <NodeState> loadReport;


/********* basis Task for Recovery ***/

class RecoveryAbleTask : public Thread {
public:
    CommBuffer<Context> lastContext; // has to be set as subscriber by the final user
    Context myContext, externContext;
    bool activated;
    virtual void init() { }
    void run(); // from Threnad
    virtual void performTask() { }; // called on each cycle of run has to publish results
};

#define NR_OF_TASKS 3
#define TASK_X 0
#define TASK_Y 1
#define TASK_Z 2

extern RecoveryAbleTask* task[NR_OF_TASKS];

#define MY_NODE_NR() (getNodeNumber() % 10)

//extern UDPOut outMsgs;  //DEBUG
//extern char   outMsgBuf[100]; //DEBUG


#endif
