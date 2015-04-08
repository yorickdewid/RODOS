/**
* @file bytesex.h
* @date 2008/11/21 11:46
* @author Sergio Montenegro
*
* Copyright 2008 DLR
*/

//#include "rodos.h"
#include "bytesex.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

// TODO: SWAPCHARS is already in macros.h
/// exchange values of char variables (a) and (b)
#define SWAPCHARS(_a, _b) {char _tempswap = _a;   _a = _b;   _b = _tempswap; }

short swapShort(short sw_) {
    short sw = sw_;
    char *c = (char*)&sw;
    SWAPCHARS(c[0], c[1]);
    return sw;
}

long swapLong(long lw_) {
    long lw = lw_;
    char *c = (char*)&lw;
    SWAPCHARS(c[0], c[3]);
    SWAPCHARS(c[1], c[2]);
    return lw;
}

long long swapLonglong(long long llw_) {
    long long llw = llw_;
    char *c = (char*)&llw;
    SWAPCHARS(c[0], c[7]);
    SWAPCHARS(c[1], c[6]);
    SWAPCHARS(c[2], c[5]);
    SWAPCHARS(c[3], c[4]);
    return llw;
}


short shortConvertHost2Net(short sw) {
    if(isHostBigEndian) return sw;
    return swapShort(sw); 
} 

long  longConvertHost2Net(long lw) {
    if(isHostBigEndian) return lw;
    return swapLong(lw); 
}

long long longlongConvertHost2Net(long long llw) {
    if(isHostBigEndian) return llw;
    return swapLonglong(llw); 
} 



#ifndef NO_RODOS_NAMESPACE
}
#endif


