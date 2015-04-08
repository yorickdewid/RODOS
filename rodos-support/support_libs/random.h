

/**
* @file debug.h
* @date 2008/06/17 10:46
* @author Sergio MOntenegro
*
* Copyright 2008 DLR
*
* @brief simple misc functions
*/

#pragma once


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/** 32 bit random set, a value -2G .. + 2G **/
int32_t randomTT800();

/** 32 bit random set, a value 0 ...  2G **/
inline uint32_t  randomTT800Positive() { return randomTT800() & 0x7fffffff; }

static const int32_t MAX_RANDOM_TT800 = 2147483647;

// Double values normalized 0 ..1 or -1 .. 1 
inline double drandPositive()              { return (double)randomTT800Positive()  / (double)MAX_RANDOM_TT800; }
inline double drandPositive(double gain)   { return drandPositive() * gain; }
inline double drand()                      { return (double)randomTT800()  / (double)MAX_RANDOM_TT800; }
inline double drand(double gain)           { return drand()*gain; }


#ifndef NO_RODOS_NAMESPACE
}
#endif

