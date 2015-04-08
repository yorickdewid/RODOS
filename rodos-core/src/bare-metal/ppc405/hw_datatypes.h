/**
* @file hw_datatypes.h
* @date 2008/06/19 8:46
* @author Lutz Dittrich,
*         adapted in 2010 by Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2008-2010 DLR
*
* @brief typedefs for hw specific data types
*/

#ifndef RODOS_HW_DATATYPES_H
#define RODOS_HW_DATATYPES_H

#include <stdint.h>
#include <stddef.h>

// Guard type for C++ ABI
namespace __cxxabiv1 {
  typedef int32_t __guard;
}

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

typedef uint16_t TUDPPortNr;

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* RODOS_HW_DATATYPES_H */
