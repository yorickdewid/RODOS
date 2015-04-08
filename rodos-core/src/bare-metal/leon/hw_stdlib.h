/* -*- Mode: C++ -*- */

/*********************************************************** Copyright
 **
 ** Copyright (c) 2008-2010, German Aerospace Center (DLR)
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/


/**
* @file
* @date 2010/08/02
* @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2008-2010 DLR
*
* @brief Declare printf, vprintf, memcpy.
*
*/

#ifndef RODOS_HW_STDLIB_H
#define RODOS_HW_STDLIB_H

#include <stdarg.h>
#include "hw_datatypes.h"

struct FILE {
   int fd;
};

#ifndef RAND_MAX
#define RAND_MAX 0x7fffffff
#endif

/**
 * A simple xorshift random generator */
struct RandomState {
private:
  unsigned x, y, z, w;

  static RandomState globalState;

public:
  /**
   * Initialise random generator with current time */
  RandomState ();

  /**
   * Initialise random generator with a given value */
  RandomState (unsigned seed);

  /**
   * Reset this generator to a different seed. */
  void setSeed(unsigned seed);

  /**
   * Get a random number from this generator and update the state */
  int getRandom ();

  /**
   * Get the global random state */
  static RandomState *getGlobalState()
  {
    return &globalState;
  }
};


extern "C" {
  extern FILE *stdin;
  extern FILE *stdout;
  extern FILE *stderr;

  /**
   * Math library needs this: */
  extern int __errno;

#if 0
  /**
   * printf() from the printf library */
  extern int xprintf(const char *format, ...)
    __attribute__((__format__(__printf__,1,2)));

  /**
   * vprintf() from the printf library */
  extern int vprintf(const char *format, va_list ap)
    __attribute__((__format__(__printf__,1,0)));

  /**
   * fflush() from the printf library */
  extern int fflush(FILE *);

  /**
   * strlen() from the printf library */
  extern size_t strlen (char const *s);

  /**
   * strtol() from the printf library */
  extern long strtol (char const *s, char **endptr, int base);

  /**
   * strtoul() from the printf library */
  extern unsigned long strtoul (char const *s, char **endptr, int base);

  /**
   * memmove() from the printf library */
  extern void* memmove (void* dest, const void* src, int n);

  /**
   * memcpy() is used in printf implementation (and in some rodos headers)
   *
   * implemented in hw_stdlib.cpp */
  extern void* memcpy (void* dest, const void* src, int n);

  /**
   * strncpy() is used by this kernel port
   *
   * implemented in hw_stdlib.cpp */
  extern char* strncpy (char* dest, const char* src, size_t n);

  /**
   * strcmp() standard function
   *
   * implemented in hw_stdlib.cpp */
  extern int strcmp (char const *a, char const *b);

  /**
   * strncmp() standard function
   *
   * implemented in hw_stdlib.cpp */
  extern int strncmp (char const *a, char const *b, size_t);

  /**
   * rand(): random generator.  Internally, this uses
   * RandomState.getRandom() with one global state for all threads.
   *
   * For thread-local states, use your own RandomState object.
   */
  extern int rand(void);

  /**
   * Reset the global random generator */
  extern void srand (unsigned seed);

  /**
   * For completeness of the POSIX function family: random generator with single 32-bit seed.
   * This isn't as good as the random generators based on RandomState.
   */
  extern int rand_r(unsigned *seed);

  /**
   * Fast copy of 8-bit aligned data */
  void memCopy64 (
      unsigned long long *dst,
      unsigned long long const *src,
      unsigned long long const *dst_end);

  /**
   * Fast clear of 8-bit aligned data */
  void memClear64 (
      unsigned long long *dst,
      unsigned long long const *dst_end);

#endif

  // TODO JM moved this here to be included by the preprocessor
  /**
     * rand(): random generator.  Internally, this uses
     * RandomState.getRandom() with one global state for all threads.
     *
     * For thread-local states, use your own RandomState object.
     */
    extern int rand(void);

    /**
     * Reset the global random generator */
    extern void srand (unsigned seed);

    /**
     * For completeness of the POSIX function family: random generator with single 32-bit seed.
     * This isn't as good as the random generators based on RandomState.
     */
    extern int rand_r(unsigned *seed);

}


#endif /* RODOS_HW_STDLIB_H */
