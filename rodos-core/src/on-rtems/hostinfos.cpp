
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

bool  getIsHostBigEndian()          { return isHostBigEndian; }
long  getSpeedKiloLoopsPerSecond()  { return 10000;       } // see .../development-tests/cspuspeedtest.cpp 
long  getYieldTimeOverhead()        { return 40000        } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()        { return "leon";      } 
const char* getHostBasisOS()        { return "rtems";     }


#ifndef NO_RODOS_NAMESPACE
}
#endif

