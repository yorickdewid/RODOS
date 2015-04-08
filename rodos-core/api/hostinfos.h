
/**
* @file hostinfos.h
* @author Sergio MOntenegro
*/

#ifndef __HOSTINFOS_H_
#define __HOSTINFOS_H_


/**  bigendian is standard net potocoll. 
 * Warning: Will be set in main.
 * never use beore main (in static constructurs)
 **/


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

extern bool getIsHostBigEndian();          ///< never use before main()
extern long  getSpeedKiloLoopsPerSecond(); ///< ony an aproximation, max differ +/- 50%
extern long  getYieldTimeOverhead();       ///< number of nanoseconds for a thread switch
extern const char* getHostCpuArch(); 
extern const char* getHostBasisOS();

extern volatile int64_t idleCnt; ///< a hint about cpu idle time, cnt will be incremented bei idleThread

#ifndef NO_RODOS_NAMESPACE
}
#endif


#endif /* __HOSTINFOS_H */

