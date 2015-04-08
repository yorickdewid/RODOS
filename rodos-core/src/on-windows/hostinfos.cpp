
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

bool  getIsHostBigEndian()          { return isHostBigEndian; }
long  getSpeedKiloLoopsPerSecond()  { return 350000;      } // see .../development-tests/cspuspeedtest.cpp 
long  getYieldTimeOverhead()        { return 990;         } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()        { return "x86";       } 
const char* getHostBasisOS()        { return "windows";   }


#ifndef NO_RODOS_NAMESPACE
}
#endif


