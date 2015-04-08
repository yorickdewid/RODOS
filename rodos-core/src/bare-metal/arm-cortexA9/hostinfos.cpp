#include "hostinfos.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

bool getIsHostBigEndian(){
	return 0;
}

long getSpeedKiloLoopsPerSecond(){ return 43607; } // see .../development-tests/cspuspeedtest.cpp
long getYieldTimeOverhead()      { return 8000;  } // see tutorials/core/yieldtime.cpp

const char* getHostCpuArch(){
	return "ARMv7-a";
}

const char* getHostBasisOS(){
	return "Bare-Metal";
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
