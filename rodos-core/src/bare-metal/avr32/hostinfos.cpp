
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"
namespace RODOS {
bool  getIsHostBigEndian()          { return isHostBigEndian; }
long  getSpeedKiloLoopsPerSecond()  { return 2852;       } // see .../development-tests/cspuspeedtest.cpp
long  getYieldTimeOverhead()        { return 122000;     } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()        { return "avr";      }
const char* getHostBasisOS()        { return "baremetal";}
}


