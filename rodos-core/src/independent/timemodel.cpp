

/*********************************************************** Copyright
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/


/**
 * @file time.cc
 * @date 2008/04/24 11:03
 * @author Lutz Dittrich, Sergio Montenegro
 *
 * Copyright 2008 DLR
 *
 * @brief basic functions for time handling
 *
 */

#include "rodos.h"
#include "hw_specific.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

TimeModel sysTime;

/* Constructor */
TimeModel::TimeModel() {
    utcDeltaTime = 0LL;
    tSync = 0LL;
    drift = 0.0;
    leapSeconds = 15; // this was in 2011
}

/* current time in localTime units (eg. nanoseconds) */
int64_t TimeModel::getNanoseconds() {
    return hwGetNanoseconds();
}

/* current UTC time */
int64_t TimeModel::getUTC() {
    return localTime2UTC(NOW());
}

/* set the UTC drift parameter of the clock model */
void TimeModel::setDrift(const double drift) {
    this->drift = drift;
}

/* Sets the offset and reference epoch of the clock model */
void TimeModel::setClockOffset(const int64_t utcDelta,
                               const int64_t tSync) {
    this->utcDeltaTime = utcDelta;
    this->tSync = tSync;
}

/* like setClockOffset */
void TimeModel::setUTC(const int64_t time)  {
    utcDeltaTime = time - NOW();
    tSync = NOW();
}

/* Sets diff UTC-getTime (it should be > 0 if date > 1.1.2000) */
void TimeModel::setUTCDeltaTime(const int64_t utcDelta) {
    this->utcDeltaTime = utcDelta;
    tSync = NOW();
}

/* diff between local time and UTC (>0 if after year 2000) */
int64_t TimeModel::getUTCDeltaTime() {
    return utcDeltaTime;
}

/* increments or decrements utcDeltaTime */
void TimeModel::correctUTCDelta(const int64_t deltaDelta) {
    utcDeltaTime += deltaDelta;
    tSync = NOW();
}

/* converts the given local time to the corresponding UTC time */
int64_t TimeModel::localTime2UTC(const int64_t t) {
    int64_t retval = utcDeltaTime + t +
                   (int64_t)(drift * (long double) (t - tSync) + 0.5);
    return  retval;
}

/* converts the given UTC time to the corresponding local time. */
int64_t TimeModel::UTC2LocalTime(const int64_t utc) {
    long double local = utc - utcDeltaTime + (drift * (long double)tSync);
    return (int64_t) (local / (long double)(1.0 + drift) + 0.5);
}


/* (c) Oliver Montenbruch (DLR) */
/* split given time in localTime units to year, month, day, hour, minute and
    second parts*/
void TimeModel::localTime2Calendar(const int64_t localTime,
                                   int32_t& year,
                                   int32_t& month,
                                   int32_t& day,
                                   int32_t& hour,
                                   int32_t& min,
                                   double& sec ) {

    /* Variables */
    int32_t a, b, c, d, e, f;
    int64_t localTimeUnitsOfTheDay;
    int32_t days;

    days = (int32_t)(localTime / (1*DAYS));
    localTimeUnitsOfTheDay = localTime % (1*DAYS);

    /* Convert Julian day number to calendar date */
    a = 2451545 + days;
    b = int32_t((a-1867216.25)/36524.25);
    c = a + b - (b/4) + 1525;

    d = int32_t ( (c-122.1)/365.25 );
    e = 365*d + d/4;
    f = int32_t( (c-e)/30.6001);

    day = c - e - int32_t(30.6001*f);
    month = f - 1 - 12*(f/14);
    year = d - 4715 - ((7+month)/10);

    hour = (int32_t)(localTimeUnitsOfTheDay / (1*HOURS));
    min  = (int32_t)((localTimeUnitsOfTheDay / (1*MINUTES))%60);
    sec = (localTimeUnitsOfTheDay / (1*SECONDS))%60 +
          (localTimeUnitsOfTheDay % (1*SECONDS)) / (1.0*SECONDS);
}

double TimeModel::localTime2mjd_UTC(const int64_t &localTime) {
    return (localTime / (1.0f*DAYS)) + 51544.0f;
}

/** computes the localTime units fomr 1. January 2000 to given date */
int64_t TimeModel::calendar2LocalTime(int32_t year,
                                    int32_t month,
                                    int32_t day,
                                    int32_t hour,
                                    int32_t min,
                                    double sec) {
    int32_t daysFrom2000 = 367L*year-7L*
                        (year+((month+9)/12))/4+((275*month)/9)+day-730531L;

    double secondsOfDay = (hour*60 +min)*60 + sec;

    return (daysFrom2000*DAYS) + (int64_t)(secondsOfDay*SECONDS);
}

double TimeModel::calendar2mjd_UTC( const uint16_t &year,
                                const uint8_t &month,
                                const uint8_t &day,
                                const uint8_t &hour,
                                const uint8_t &minute,
                                const double &second ) {

    uint32_t Mjd_0h;
    double fraction;

    uint8_t month_mod = month;
    uint16_t year_mod = year;

    if (month_mod<=2) {
        month_mod+=12;
        year_mod--;
    }
    if ( (10000L*year_mod+100L*month+day) <= 15821004L  ) {
        // julian
        Mjd_0h = 365L*year_mod - 679004L + -2 + ((year_mod+4716)/4) - 1179 + int(30.6001*(month+1)) + day;
    } else {
        // gregorian 
        Mjd_0h = 365L*year_mod - 679004L + (year_mod/400)-(year_mod/100)+(year_mod/4) + int(30.6001*(month_mod+1)) + day;
    }

    fraction = (hour+minute/60.0f+second/3600.0f) / 24.0f;
    return Mjd_0h + fraction;
}

void TimeModel::mjd_UTC2calendar( const double &MJD_UTC,
                               uint16_t &year,
                               uint8_t &month,
                               uint8_t &day,
                               uint8_t &hour,
                               uint8_t &minute,
                               double &second ) {
    uint32_t a;
    double q = 0, b = 0, c = 0, hour_frac = 0;
    int32_t d = 0, e = 0, f = 0;

    // JD at noon
    a = static_cast<int32_t> (MJD_UTC) + 2400001L;
    // fraction of the day
    q = MJD_UTC - static_cast<int32_t>(MJD_UTC);

    b = static_cast<int32_t>(a-1867216.25f)/36524.25f; // gregorian calendar assumed,
    c = a + b - (b/4) + 1525;
    d = static_cast<int32_t>((c-121.1)/365.25f);
    e = static_cast<int32_t>(365.25*d);
    f = static_cast<int32_t>((c-e) / 30.6001f);

    day = static_cast<uint8_t>(c - e - static_cast<uint8_t>(30.6001f*f));
    month = static_cast<uint8_t>(f - 1 - 12*(f/14));
    year = static_cast<uint16_t>(d - 4715 - (7+month)/10);
    hour =  static_cast<uint8_t>(q*24.0f);
    hour_frac = q*24.0f-hour;
    minute = static_cast<uint8_t>( (hour_frac)*60.0f );
    second = (hour_frac*60.0f - minute)*60.0f;
}

double TimeModel::mjd_UT1fromUTC( const double &MJD_UTC, const double &UT1_UTC ) {

    return MJD_UTC + UT1_UTC/86400.0f;
}

double TimeModel::mjd_TTfromUT1( const double &MJD_UT1, const double &UT1_UTC, const double &UTC_TAI ) {

    return MJD_UT1 + (32.184f - UTC_TAI- UT1_UTC)/86400.0f;
}

int64_t TimeModel::computeNextBeat(const int64_t begin,
                                 const int64_t period,
                                 const int64_t timeNow) {

    if (begin > timeNow) return begin;
    if (period == 0)     return END_OF_TIME;;
    
    return   ((((timeNow-begin)/period)+1)*period)+begin;
}

/**
 * Split the local time representation
 * in the ECSS / CCSDS time representation standard (CUC)
 */
void TimeModel::splitTimeECSS(int64_t localTime, uint32_t &seconds, uint32_t &fraction) {
    seconds = (uint32_t)(localTime / SECONDS);
    int64_t preFraction	= localTime - (seconds*SECONDS);
    preFraction 		= preFraction << 32;
    fraction = (uint32_t)(preFraction / SECONDS);
}



#ifndef NO_RODOS_NAMESPACE
}
#endif

