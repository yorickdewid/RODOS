
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"

bool  getIsHostBigEndian()          { return isHostBigEndian; }
//long  getSpeedKiloLoopsPerSecond()  { return 163; } // 40 MHz
long  getSpeedKiloLoopsPerSecond()  { return 208;        } // 60 MHz
long  getYieldTimeOverhead()        { return 168000;     } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()        { return "leon3";    }
const char* getHostBasisOS()        { return "baremetal";}



