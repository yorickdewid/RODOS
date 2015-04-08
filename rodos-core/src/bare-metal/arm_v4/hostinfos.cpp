
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"
namespace RODOS {
bool  getIsHostBigEndian()          { return isHostBigEndian; }
//long  getSpeedKiloLoopsPerSecond()  { return 2994;        } // see .../development-tests/cspuspeedtest.cpp 
long  getSpeedKiloLoopsPerSecond()  { return 1941;        } // -O0 (see .../development-tests/cspuspeedtest.cpp)
long  getYieldTimeOverhead()        { return 180000;      } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()        { return "arm7tdmi";  } 
const char* getHostBasisOS()        { return "baremetal"; }
}

