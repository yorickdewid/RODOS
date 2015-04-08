
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
long  getSpeedKiloLoopsPerSecond()  { return 62650;  } // see .../development-tests/cspuspeedtest.cpp
long  getYieldTimeOverhead()        { return 5000;   } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()        { return "raspberrypi";    }
const char* getHostBasisOS()        { return "baremetal"; }


#ifndef NO_RODOS_NAMESPACE
}
#endif

