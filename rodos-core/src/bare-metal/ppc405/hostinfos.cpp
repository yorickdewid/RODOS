/**
 * @brief RODOS interfaces to get host information.
 *
 * To compute this value please use tutorials/development-tests/cpuspeedtest.cpp
 */


#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

bool  getIsHostBigEndian()          { return isHostBigEndian; }
long  getSpeedKiloLoopsPerSecond()  { return 5254;        } // see .../development-tests/cspuspeedtest.cpp 
long  getYieldTimeOverhead()        { return 83000;       } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()        { return "ppc";       } 
const char* getHostBasisOS()        { return "baremetal"; }



#ifndef NO_RODOS_NAMESPACE
}
#endif


