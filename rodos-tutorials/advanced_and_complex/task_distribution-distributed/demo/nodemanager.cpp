#include "rodos.h"
#include "globals.h"
#include "mask-screen.h"
#include "mask.h"

/******************************/

#define MIN(a,b) (a) < (b) ? (a) : (b)

static Application nodeManagerApp("nodeManager", 6000);

static Fifo<NodeState, 100> externalNodes;
static Subscriber nameNotImportant(loadReport, externalNodes);

/*************************************************/
#define MAX_NUME_OF_NODES 10
class NodeManager : public Thread {
public:
    NodeState myNodeState;
    long myLast;
    NodeState nodeState, externNodeState;
    int otherNodesLoad[MAX_NUME_OF_NODES];
    long long otherNodesReportTime[MAX_NUME_OF_NODES];
    void run();
} nodeManager;  


void NodeManager::run () {
   
    myNodeState.nodeNr = MY_NODE_NR();
    for(int i = 0; i < MAX_NUME_OF_NODES; i++) {
	otherNodesLoad[i] = 99;
	otherNodesReportTime[i] = 0;
    }

    INIT_MASK();
    PRINTF("%s%d\n", NR, myNodeState.nodeNr);


    TIME_LOOP(2200*MILLISECONDS, 500*MILLISECONDS) {
        long long timeNow = NOW();
        long myLoad = 0;
         for(int iter = 0;  iter < NR_OF_TASKS; iter++) { myLoad += task[iter]->activated; }

        myNodeState.reportTime = NOW();
        myNodeState.cpuLoad = myLoad;
        loadReport.publish(myNodeState);

        while(externalNodes.get(externNodeState)) { // read all reports from other nodes
           otherNodesReportTime[externNodeState.nodeNr] = timeNow;
           otherNodesLoad[externNodeState.nodeNr] = externNodeState.cpuLoad; 
	}
        for(int i = 0; i < MAX_NUME_OF_NODES; i++) { // rm older reports
            if(otherNodesReportTime[i] + 1*SECONDS < timeNow) otherNodesLoad[i] = 99;
        }

	int minLoad = 99;
        for(int i = 0; i < MAX_NUME_OF_NODES; i++) {
            minLoad = MIN(otherNodesLoad[i], minLoad);
        }
        minLoadGlobal = minLoad;
        myLoadGlobal = myLoad;
        
    }

}


