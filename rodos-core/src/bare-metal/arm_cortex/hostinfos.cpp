
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"

bool  getIsHostBigEndian()          { return isHostBigEndian; }
long  getSpeedKiloLoopsPerSecond()  { return 2932;        } // see .../development-tests/cspuspeedtest.cpp 
long  getYieldTimeOverhead()        { return 120000;      } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()        { return "cortex-M3 (LM3S)";    } 
const char* getHostBasisOS()        { return "baremetal"; }



