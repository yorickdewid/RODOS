
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

bool  getIsHostBigEndian()          { return isHostBigEndian; }
long  getSpeedKiloLoopsPerSecond()  { return 4022;       } // see .../development-tests/cspuspeedtest.cpp 
long  getYieldTimeOverhead()        { return 86000;      } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()        { return "cortex-M3 (STM32)";    } 
const char* getHostBasisOS()        { return "baremetal"; }



#ifndef NO_RODOS_NAMESPACE
}
#endif


