
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"

bool  getIsHostBigEndian()          { return isHostBigEndian; }
long  getSpeedKiloLoopsPerSecond()  { return 50000;      } // see .../development-tests/cspuspeedtest.cpp 
long  getYieldTimeOverhead()        { return 7000;       } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()        { return "leon3";    }
const char* getHostBasisOS()        { return "baremetal";}



