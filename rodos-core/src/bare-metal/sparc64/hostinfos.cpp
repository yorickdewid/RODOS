/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief RODOS interfaces to get host information.
 */

#include <rodos.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

bool  getIsHostBigEndian() {
  return isHostBigEndian;
}

/**
 * @brief This really makes no sense for sparc64 at the moment.
 * Usually this is computed with tutorials/development-tests/cspuspeedtest.cpp
 * but since there is no SINGLE embedded sparc64 CPU at the moment and all
 * testing systems are different, we avoid a specific value here.
 * @return negative value to make clear it is currently useless.
 */
long getSpeedKiloLoopsPerSecond() { return -1; }      // see .../development-tests/cspuspeedtest.cpp
long getYieldTimeOverhead()       { return -1; } // see tutorials/core/yieldtime.cpp
const char* getHostCpuArch()      { return "sparc64"; }
const char* getHostBasisOS()      { return "baremetal"; }


#ifndef NO_RODOS_NAMESPACE
}
#endif

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
