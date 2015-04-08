
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

bool  getIsHostBigEndian()          { return isHostBigEndian; }

const char* getHostCpuArch()        { return "avr";       } 
// avr is currently running with 12MHz only! 
// -> possible are 66MHz but it's not working -> programm crashes
// see tutorials/advanced_and_complex/development-tests/cpuspeedtest.cpp
// long  getSpeedKiloLoopsPerSecond()  { return 268;      } // Debug 
long  getSpeedKiloLoopsPerSecond()  { return 353;      } // // see .../development-tests/cspuspeedtest.cpp 
long  getYieldTimeOverhead()        { return 120000;   } // see tutorials/core/yieldtime.cpp
const char* getHostBasisOS()        { return "freeRtos";     }



#ifndef NO_RODOS_NAMESPACE
}
#endif


