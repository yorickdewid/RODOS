/* 
 * File:   fatTime.h
 * Author: Gabriel Reimers
 *
 * Created on March 26, 2012, 1:59 PM
 */

#include "integer.h"

#pragma once

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * Returns the current system time. This is used by ChaN Fat.
 * @return 
 */
DWORD get_fattime(void);

/**
 * Sets the current FAT-System time. Depending on the implementation this
 * may change the operating system time or not. 
 * @param year
 * @param month
 * @param day
 * @param hour
 * @param min
 * @param sec
 */
void setCurrentTime(int32_t year, int32_t month, int32_t day, int32_t hour,int32_t min, int32_t sec);

#ifndef NO_RODOS_NAMESPACE
}
#endif
