#include "rodos.h"
#include "globals.h"
#include "monitoringMsg.h"

#include "rodosgui-client.h"



#define RAND randomTT800Positive()

/************** Test GUI output ********************/

static Application  nameNotImportant00("MonitorGUI", TASK_MONITOR_GUI);

class MonitorGUI : public Subscriber {
public:
    MonitorGUI() : Subscriber(monitorMsgTopic, "printerGUI") { }
    long put(const long topicId, const long len, const void* msg, const NetMsgInfo& netMsgInfo) {

        MonitoringMsg* report = (MonitoringMsg*)msg;

        int par1, par2;
        report->deserialize(&par1);
        report->deserialize(&par2);

        static char buf[100];
        static int negCnt = 0;
        static int oddCnt = 0;
        static int eveCnt = 0;
        static int priCnt = 0;

        /** In this case we show different posibilities to deserialize die
        	parameters from report. All them are equivalent and may be used
        	according to convinienve
        **/

        switch (report->msgType) {

        case NEG_CNT:
            PRINTF("neg %d\n", negCnt);
            rodosGui.drawRectangle(10, 20, 500, 20);
            rodosGui.fillRectangle(10, 20, negCnt*2, 20,  255,  negCnt,  0);
            rodosGui.drawText(10,35, "NegCnt");
            negCnt++;
            break;


        case ODD_CNT: {
            PRINTF("odd %d\n", oddCnt);
            rodosGui.drawRectangle(10, 50, 500, 20);
            rodosGui.fillRectangle(10, 50, oddCnt*2, 20,  255,  oddCnt,  255);
            rodosGui.drawText(10,65, "oddCnt");
            oddCnt++;
            break;
        }
        break;

        case EVEN_CNT_PRE:
        case EVEN_CNT_POST:
            PRINTF("even %d\n", eveCnt);
            rodosGui.drawRectangle(10, 80, 500, 20);
            rodosGui.fillRectangle(10, 80, eveCnt*2, 20,  0,  255,  eveCnt);
            rodosGui.drawText(10,95, "EvenCnt");
            eveCnt++;
            break;

        case PRIM_CNT_PRE:
        case PRIM_CNT_POST:
            PRINTF("prim %d\n", priCnt);
            xsprintf(buf, "PriCnt: %4d   ", par2);
            rodosGui.drawRectangle(10, 110, 500, 20);
            rodosGui.fillRectangle(10, 110, 500, 20,  128, 128, 128);
            rodosGui.fillRectangle(10, 110, priCnt*2, 20,  priCnt,  0,  255);
            rodosGui.drawText(10,125, buf);
            priCnt++;
            break;

        default:
            PRINTF("MSG! Unknown Message %d!\n", report->msgType);

        }

        return 1;
    }

} monitorGui;



