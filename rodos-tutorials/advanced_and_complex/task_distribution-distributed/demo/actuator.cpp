#include "rodos.h"
#include "globals.h"
#include "mask-screen.h"
#include "mask.h"

/******************************/

static Application actautorApp("Actuator", 7000);

long long lastXReport = 0;
long long lastYReport = 0;
long long lastZReport = 0;

class ActuatorX :  public Subscriber {
public:
    ActuatorX() : Subscriber(topicX, "ActuatortX") { }
    long put(const long topicId, const long len, const void* msg, const NetMsgInfo& netMsgInfo) {
        Context* cont = (Context*)msg;
        PRINTF("%s%7ld%s%2d\n", ROTX, cont->value, SX, cont->nodeNr);
        lastXReport = NOW();
        return 1;
    }
} actuatorX;


class ActuatorY :  public Subscriber {
public:
    ActuatorY() : Subscriber(topicY, "ActuatortY") { }
    long put(const long topicId, const long len, const void* msg, const NetMsgInfo& netMsgInfo) {
        Context* cont = (Context*)msg;
        PRINTF("%s%7ld%s%2d\n", ROTY, cont->value, SY, cont->nodeNr);
        lastYReport = NOW();
        return 1;
    }
} actuatorY;

class ActuatorZ :  public Subscriber {
public:
    ActuatorZ() : Subscriber(topicZ, "ActuatortZ") { }
    long put(const long topicId, const long len, const void* msg, const NetMsgInfo& netMsgInfo) {
        Context* cont = (Context*)msg;
        PRINTF("%s%7ld%s%2d\n", ROTZ, cont->value, SZ, cont->nodeNr);
        lastZReport = NOW();
        return 1;
    }
} actuatorZ;



static Fifo<NodeState, 100> externalNodes;
static Subscriber nameNotImportant(loadReport, externalNodes);

/*************************************************/
extern void printLoad(const char* pos, int load);

#define MAX_NUME_OF_NODES 10
class Actuator : public Thread {
public:
    NodeState nodeReport;
    int otherNodesLoad[MAX_NUME_OF_NODES];
    long long otherNodesReportTime[MAX_NUME_OF_NODES];
    void run();
} actuator;  


void Actuator::run () {
   

    for(int i = 0; i < MAX_NUME_OF_NODES; i++) {
	otherNodesLoad[i] = 99;
	otherNodesReportTime[i] = 0;
    }

    INIT_MASK();

    TIME_LOOP(2400*MILLISECONDS, 1000*MILLISECONDS) {
        long long timeNow = NOW();
        
        if(lastXReport + 1*SECONDS < timeNow) PRINTF("%s-------\n", ROTX);
        if(lastYReport + 1*SECONDS < timeNow) PRINTF("%s-------\n", ROTY);
        if(lastZReport + 1*SECONDS < timeNow) PRINTF("%s-------\n", ROTZ);

        while(externalNodes.get(nodeReport)) { // read all reports from other nodes
           otherNodesReportTime[nodeReport.nodeNr] = timeNow;
           otherNodesLoad[nodeReport.nodeNr] = nodeReport.cpuLoad; 
	}
        for(int i = 0; i < MAX_NUME_OF_NODES; i++) { // rm older reports
            if(otherNodesReportTime[i] + 1*SECONDS < timeNow) otherNodesLoad[i] = 99;
        }

        printLoad(L0, otherNodesLoad[0]);
        printLoad(L1, otherNodesLoad[1]);
        printLoad(L2, otherNodesLoad[2]);
        printLoad(L3, otherNodesLoad[3]);
        printLoad(L4, otherNodesLoad[4]);
        printLoad(L5, otherNodesLoad[5]);
        printLoad(L6, otherNodesLoad[6]);
        printLoad(L7, otherNodesLoad[7]);
        printLoad(L8, otherNodesLoad[8]);
        printLoad(L9, otherNodesLoad[9]);
        
    }

}


void printLoad(const char* pos, int load) {
   if(load == 99 || load < 0) PRINTF("%s%s--%s",  pos, SCREEN_RED, SCREEN_BLACK);
   else                       PRINTF("%s%2d", pos, load);
}




