/**
* @file hw_stdlib.h
* @date 2010/08/02
* @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*         Adriano Carvalho: commented unnecessary stuff
*
* Copyright 2008-2010 DLR
*
* @brief Declare printf, vprintf, memcpy.
*
*/

#ifndef RODOS_HW_STDLIB_H
#define RODOS_HW_STDLIB_H

#include <stdarg.h>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

struct FILE {
  int fd;
};

// #define RAND_MAX 0x7fffffff

extern "C"
{
  extern FILE *stdin;
  extern FILE *stdout;
  extern FILE *stderr;

}

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* RODOS_HW_STDLIB_H */
