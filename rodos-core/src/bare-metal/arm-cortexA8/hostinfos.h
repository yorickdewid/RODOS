/*
 * Tiago Duarte
 * 17-04-2013 @ 11:11
 */

#ifndef __HOSTINFOS_H__
#define __HOSTINFOS_H__

#include "rodos.h"

bool  		getIsHostBigEndian();
long  		getSpeedKiloLoopsPerSecond();
const char* getHostCpuArch();
const char* getHostBasisOS();

#endif /* __HOSTINFOS_H__ */
