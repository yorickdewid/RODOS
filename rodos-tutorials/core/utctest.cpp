#include "rodos.h"




/*********************************************************************/

void MAIN(){
   int64_t timeNow;

   timeNow =  TimeModel::calendar2LocalTime(2008, 11, 12, 17, 35, 34.21);
   xprintf("Time now (now == creation of this prog) in localTime (nanoseconds)  = %lld\n", timeNow);
   sysTime.setUTC(timeNow);

}


static Application module01("utcprinter");

class PrintTime : public Thread {
public:
  PrintTime() : Thread("printtime") { }
  void init() { PRINTF("Waiting 2 seconds");}
  void run();
};

void PrintTime::run() {
    int32_t year;
    int32_t month;
    int32_t day;
    int32_t hour;
    int32_t min;
    double sec;

    uint32_t ccsdsSeconds;
    uint32_t ccsdsFraction;

    int64_t utcNow;
    TIME_LOOP(0, 2*SECONDS){
      utcNow = sysTime.getUTC();
      PRINTF("UTC in localTime (nanoseconds)  = %lld  ", utcNow);
      TimeModel::localTime2Calendar(utcNow,year,month,day,hour,min,sec);
      PRINTF("  -> %d.%d.%d  %d:%d:  %d.%03d\n",
                                          day, month, year, hour, min, (int32_t)sec, (int32_t)(1000*(sec-(int32_t)sec))  );

      TimeModel::splitTimeECSS(NOW(), ccsdsSeconds, ccsdsFraction);
	  PRINTF("CCSDS Time = (%d 0x%08x).(%d 0x%08x)\n", ccsdsSeconds,ccsdsSeconds, ccsdsFraction,ccsdsFraction); 
    }
}

static PrintTime printTime;


