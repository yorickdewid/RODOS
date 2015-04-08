
#include "hal/hal_sharedmemory.h"

#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


HAL_Sharedmemory::HAL_Sharedmemory(Sharedmemory_IDX shmIdx) {}

void * HAL_Sharedmemory::init(int32_t * size, int32_t * maxMembers){return NULL;}

int32_t HAL_Sharedmemory::lock() { return false; }

int32_t HAL_Sharedmemory::islocked() { return true; }

void HAL_Sharedmemory::unlock() {}

int32_t HAL_Sharedmemory::getMemberId() { return 0; }

void HAL_Sharedmemory::raiseSharedMemoryChanged() {}

bool HAL_Sharedmemory::isWriteFinished() { return false; }

bool HAL_Sharedmemory::isDataReady() { return false; }

void HAL_Sharedmemory::resetDataReady() { }

#ifndef NO_RODOS_NAMESPACE
}
#endif
