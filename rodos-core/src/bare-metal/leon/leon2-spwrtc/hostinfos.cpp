
/** READ THIS!!!! **/
/** To compute this value please use tutorials/development-tests/cpuspeedtest.cpp **/

#include "rodos.h"

bool  getIsHostBigEndian()          { return isHostBigEndian; }
long  getSpeedKiloLoopsPerSecond()  { return 1563;      } // see .../development-tests/cspuspeedtest.cpp
const char* getHostCpuArch()        { return "leon2";    } 
const char* getHostBasisOS()        { return "baremetal"; }



