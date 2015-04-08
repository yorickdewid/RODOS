#include "rodos.h"
#include "globals.h"

#include "stdio.h" // for sscanf

/******************************/

static Application taskDistributor("taskDistributor", TASK_DISTRIBUTOR);

static Fifo<NodeState, MAX_NUMBER_OF_NODES+3> nodesState;
static Subscriber nameNotImportant(nodeStateTopic, nodesState);


/******************* subscriber ******************/

static NodeState nodeStateTable[MAX_NUMBER_OF_NODES];
static DistributionTable distributionTable;

/************ Core thread ************/

class TaskDistributorCore : public Thread {
    NodeState nodeStateReport;

    /********************************************/
    void initTables() {
	for(int i = 0; i < NUM_OF_TASKS; i++) {
   	    distributionTable.nodeToExecute[i] = -1;
	}
	for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) {
   	    nodeStateTable[i].nodeSane   = false;
   	    nodeStateTable[i].reportTime = 0;
   	    nodeStateTable[i].cpuLoad    = 0;
	}
    }

    /****************************************/
    void executeUserCommands() {
            char* userCommand = getsNoWait(); 
            if(userCommand == 0) return;

	    unsigned int task;
	    int node;
	    sscanf(userCommand,"%u %d", &task, &node);
	    PRINTF(SCREEN_MOVE_CURSOR_TO, 6, 20);
	    if(task >= FIRST_USERTASK && task < NUM_OF_TASKS) {
	        distributionTable.nodeToExecute[task] = node;
	        PRINTF(" Task %2u in Node %2d   \n", task, node);
	    } else { // Erroneus command
	        PRINTF(" Task %s%2u%s in Node %2d  \n", SCREEN_RED, task, SCREEN_BLACK, node);
	    }
    }


    /*****************************************/
    void automaticTaskMigratoion() {

	    int minLoad = NUM_OF_TASKS +1;
	    int maxLoad = 0;
	    int minLoadNode = -1;
	    int maxLoadNode = -1;

	    /******************* find minimal and maximal cpu load ***/
	    for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) {
                if(!nodeStateTable[i].nodeSane) continue;
		if(nodeStateTable[i].cpuLoad < minLoad) {
			minLoad = nodeStateTable[i].cpuLoad;
			minLoadNode = i;
		}
		if(nodeStateTable[i].cpuLoad > maxLoad) {
			maxLoad = nodeStateTable[i].cpuLoad;
			maxLoadNode = i;
		}
	    }

	    /**** Distribute home-less taks **/
	    bool homelessTasksDistributed = false;
	    for(int i = FIRST_USERTASK; i < NUM_OF_TASKS; i++) {
		int homeNode =distributionTable.nodeToExecute[i];
   	        if(homeNode == -1 || !nodeStateTable[homeNode].nodeSane) {
   	            distributionTable.nodeToExecute[i] = minLoadNode;
	            homelessTasksDistributed = true;
		}
	    }

	   if(homelessTasksDistributed) return; // no more actions

	    /*** If the load difference is to big: migrate one tasks **/
	    if(maxLoad == 0 ) return;
	    if(minLoadNode == -1 || maxLoadNode == -1) return;
	    if(maxLoad - minLoad < 2) return; 

	    for(int i = FIRST_USERTASK; i < NUM_OF_TASKS; i++) {
   	        if(distributionTable.nodeToExecute[i] == maxLoadNode) {
   	            distributionTable.nodeToExecute[i] = minLoadNode ;
		    return;
		}
	    }
    }




public:
    TaskDistributorCore() : Thread("TaskDistributorCore") { }

    void run () {

	initTables();


	TIME_LOOP(900*MILLISECONDS, 500*MILLISECONDS) {

	    TTime timeNow = NOW();
 
	    executeUserCommands();

	    /** Get reports from task managers **/
            PRINTF(SCREEN_MOVE_CURSOR_TO, 7, 20);
	    PRINTF(SCREEN_CLEAR_LINE);
	    while(nodesState.get(nodeStateReport)) {
                unsigned int nodeNr = nodeStateReport.nodeNumber % MAX_NUMBER_OF_NODES;
		PRINTF(" %2d:%ld ", nodeNr, nodeStateReport.cpuLoad);
		if(nodeNr < MAX_NUMBER_OF_NODES) {
                    nodeStateTable[nodeNr] = nodeStateReport;
                    nodeStateTable[nodeNr].reportTime = timeNow; // Overwrite with my own time
		}
	    }

	    /** Analyse which nodes are sane **/
            PRINTF(SCREEN_MOVE_CURSOR_TO, 5, 20);
	    TTime deadLine = timeNow - 750*MILLISECONDS;
	    for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) {
	        if(nodeStateTable[i].reportTime < deadLine) {
		    nodeStateTable[i].nodeSane = false;
		}
	        if(nodeStateTable[i].nodeSane) PRINTF(" **");
		else PRINTF(" %s--%s", SCREEN_RED, SCREEN_BLACK);
	    }

            automaticTaskMigratoion();

	    /*** Now distribute the distribution table **/
	    distributionTableTopic.publish(distributionTable);

	    /** go to line where user may type commands **/
            PRINTF(SCREEN_MOVE_CURSOR_TO, 9, 20);

	} // timeloop: Every second
   } // run
} taskDistirbutorCore;


/***************************************/

void MAIN() {

    PRINTF("\n\n-------------------\n");

    PRINTF("Press return to start\n");
    getchar();
    PRINTF(SCREEN_CLEAR);

    PRINTF(SCREEN_MOVE_CURSOR_TO, 2, 1);
    PRINTF("TaskDistributor on node %ld (== master node)\n", getNodeNumber()); 
    PRINTF("distibution table:\n"); 
    PRINTF("node number:       ");
    for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) { PRINTF(" %2d", i); }
    PRINTF("\nnode active: \n");
    PRINTF("Parsed Command: \n"); 
    PRINTF("Node Reports: \n\n");
    PRINTF("Type your command: \n");

}



