/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief RODOS related typedefs.
 * .
 */

#ifndef _SPARC64_DATATYPES_H
#define _SPARC64_DATATYPES_H

#include <stdint.h>
#include <stddef.h>
#include <types.h>

// Guard type for C++ ABI
namespace __cxxabiv1 {
  typedef int64_t __guard;
}

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

typedef uint16_t TUDPPortNr;

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* _SPARC64_DATATYPES_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
