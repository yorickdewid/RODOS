
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

bool  getIsHostBigEndian()          { return isHostBigEndian; }
long  getSpeedKiloLoopsPerSecond()  { return 8474;       } // see .../development-tests/cspuspeedtest.cpp
const char* getHostCpuArch()        { return "Bfin on MFC2 (BF561)";    }
const char* getHostBasisOS()        { return "baremetal"; }



#ifndef NO_RODOS_NAMESPACE
}
#endif
