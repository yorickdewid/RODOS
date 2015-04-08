/* 
 * File:   fatTime.cpp
 * Author: Gabriel Reimers
 * 
 * Created on March 26, 2012, 1:59 PM
 */

#include "fatTime.h"
#include "rodos.h"
#include "timemodel.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


void setCurrentTime(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t min, int32_t sec) {
    TTime time = TimeModel::calendar2LocalTime((int32_t) year, (int32_t) month, (int32_t) day, (int32_t) hour, (int32_t) min, (double) sec);
    sysTime.setUTC(time);
}

DWORD get_fattime(void) {
    DWORD res;

    TTime time = sysTime.getUTC();
    int32_t year = 2000;
    int32_t month = 1;
    int32_t day = 1;
    int32_t hour = 12;
    int32_t min = 0;
    double sec = 0;
    TimeModel::localTime2Calendar(time, year, month, day, hour, min, sec);
    int32_t secInt = (int32_t) sec;
    res = (((DWORD) year - 1980) << 25) //years since 1980
            | ((DWORD) month << 21)
            | ((DWORD) day << 16)
            | (WORD) (hour << 11)
            | (WORD) (min << 5)
            | (WORD) (secInt >> 1); //2sec resolution of FAT

    return res;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

