
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

bool  getIsHostBigEndian()          { return isHostBigEndian; }

/* see .../development-tests/cspuspeedtest.cpp
 * !!! speed value depends on compiler optimization !!!
 */
//long  getSpeedKiloLoopsPerSecond()  { return 4537;  } // -00
//long  getSpeedKiloLoopsPerSecond()  { return 10493; } // -01, -O2, -03
long  getSpeedKiloLoopsPerSecond()  { return 6995;  } // see .../development-tests/cspuspeedtest.cpp
long  getYieldTimeOverhead()        { return 50000; } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()        { return "cortex-M4 (STM32F4)";    }
const char* getHostBasisOS()        { return "baremetal"; }




#ifndef NO_RODOS_NAMESPACE
}
#endif

