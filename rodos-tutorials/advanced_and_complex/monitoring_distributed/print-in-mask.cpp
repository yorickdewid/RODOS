#include "rodos.h"
#include "globals.h"
#include "monitoringMsg.h"

#include "mask.h"

static long long last_NEG_CNT = 0;
static long long last_ODD_CNT = 0;
static long long last_EVEN_CNT_PRE = 0;
static long long last_EVEN_CNT_POST = 0;
static long long last_PRIM_CNT_PRE = 0;
static long long last_PRIM_CNT_POST = 0;
static long long last_ODDEVEN_CNT = 0;


static Application  nameNotImportant00("Monitor", TASK_MONITOR);

class MonitorPrinterMask : public Subscriber {
public:
       MonitorPrinterMask() : Subscriber(monitorMsgTopic, "printerMaks") { }
       long put(const long topicId, const long len, const void* msg, const NetMsgInfo& netMsgInfo) {

        MonitoringMsg* report = (MonitoringMsg*)msg;

        int par1, par2, par3, par4;
        par1 = par2 = par3 = par4 = 0;
        int errCnt = 0;


        /** In this case we show different posibilities to deserialize die
        	parameters from report. All them are equivalent and may be used
        	according to convinienve
        **/

        switch (report->msgType) {

        case NEG_CNT:
            last_NEG_CNT = NOW();
            PRINTF("%s%3.9f\n", NEGT, (double)report->sendTime/SECONDS);
            break;


        case ODD_CNT: {
            last_ODD_CNT = NOW();
            long cnt = report->deserializeLong();
            double odd = report->deserializeDouble();
            PRINTF(OST "%ld\n", cnt); // be awere: "aaa" "bbb" niche feature form C++ 
            PRINTF(OCNT "%d\n", (int)odd);
        }
        break;

        case EVEN_CNT_PRE:
        case EVEN_CNT_POST:
            last_EVEN_CNT_PRE = last_EVEN_CNT_POST = NOW();
            report->deserialize(&par1);
            report->deserialize(&par2);
            PRINTF("%s%d\n",ECNT, par1);
            PRINTF("%s%d\n",EPRE, par2);
            break;

        case PRIM_CNT_PRE:
        case PRIM_CNT_POST:
            last_PRIM_CNT_PRE = last_PRIM_CNT_POST = NOW();
            (*report) >> par1 >> par2;
            PRINTF("%s%d",  PRIMTYPE, report->msgType);
            PRINTF("%s%d",  PRIM1,  par1);
            PRINTF("%s%d",  PRIM2,  par2);
            break;

        case ODDEVEN_CNT:	{
            last_ODDEVEN_CNT = NOW();
            int cnt = report->deserializeInt();
            char*s  = report->deserializeString();
            int lb  = report->deserializeInt();
            PRINTF("%s%d%s%5s%s%d\n", CNT, cnt, NAME, s, LB, lb);     
        }
        break;

        default:
            PRINTF("%s%d%s%d\n", ERR, errCnt, ERRT, report->msgType);
            errCnt++;
        }

        return 1;
    }

} monitorPrinterMaks;


/***************** timeouts monitoring **********/

class TimeOutMonitor : public Thread {
  void run() {
     INIT_MASK();

    TIME_LOOP(5*SECONDS, 2*SECONDS) {
        long long timeOut = NOW() - 2*SECONDS;

        if(last_NEG_CNT < timeOut) { PRINTF("%s---------\n", NEGT); }


        if(last_ODD_CNT < timeOut) { 
            PRINTF("%s-----\n", OST);
            PRINTF("%s-----\n", OCNT);
        }

        if(last_EVEN_CNT_PRE < timeOut) {
            PRINTF("%s----\n",ECNT);
            PRINTF("%s----\n",EPRE);
        }   

        if(last_PRIM_CNT_PRE < timeOut) {
            PRINTF("%s----",  PRIMTYPE);
            PRINTF("%s----",  PRIM1);
            PRINTF("%s----",  PRIM2);
        }

        if(last_ODDEVEN_CNT < timeOut) { 
            PRINTF("%s----%s-----%s----\n", CNT, NAME, LB);
        }
    }
  }
} timeOutMonitor;


