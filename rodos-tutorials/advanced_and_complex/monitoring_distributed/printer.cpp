#include "rodos.h"
#include "globals.h"
#include "monitoringMsg.h"


/****** Shortcuts ***/


static Application  nameNotImportant00("Monitor", TASK_MONITOR);

class MonitorPrinter : public Subscriber {
public:
    MonitorPrinter() : Subscriber(monitorMsgTopic, "printer") { }
    long put(const long topicId, const long len, const void* msg, const NetMsgInfo& netMsgInfo) {

        MonitoringMsg* report = (MonitoringMsg*)msg;

        int par1, par2, par3, par4;
        par1 = par2 = par3 = par4 = 0;


        /** In this case we show different posibilities to deserialize die
        	parameters from report. All them are equivalent and may be used
        	according to convinienve
        **/

        switch (report->msgType) {

        case NEG_CNT:
            PRINTF("NEG: negcnt still running at time %3.9f\n", (double)report->sendTime/SECONDS);
            break;


        case ODD_CNT: {
            long cnt = report->deserializeLong();
            double odd = report->deserializeDouble();
            PRINTF("ODD: oddcnt odd step %ld, odd = %d\n", cnt, (int)odd);
        }
        break;

        case EVEN_CNT_PRE:
        case EVEN_CNT_POST:
            report->deserialize(&par1);
            report->deserialize(&par2);
            PRINTF("CNT: evencnt even counter step %d = corresponding evenNr %d\n", par1, par2);
            break;

        case PRIM_CNT_PRE:
        case PRIM_CNT_POST:
            (*report) >> par1 >> par2;
            PRINTF("PIM: prim-finder report id %d, primtIndex = %d, last prim Found = %d\n", report->msgType, par1, par2);
            break;

        case ODDEVEN_CNT:	{
            int cnt = report->deserializeInt();
            char*s  = report->deserializeString();
            int lb  = report->deserializeInt();
            PRINTF("OEC: ODDEVEN_cnt %d is '%5s', lastBit = %d\n", cnt, s, lb);
        }
        break;

        default:
            PRINTF("MSG! Unknown Message %d!\n", report->msgType);

        }

        return 1;
    }

} monitorPrinter;



