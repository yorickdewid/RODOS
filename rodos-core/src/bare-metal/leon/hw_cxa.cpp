/* -*- Mode: C++ -*- */

/**
* @file
* @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2008-2010 DLR
*
* @brief Auxiliary stuff for gcc.
*
*/

#include <rodos.h>
#include "hw_core.h"
#include "hw_stdlib.h"
#include "hw_trap.h"
#include "hw_cxa.h"


void * __dso_handle= &__dso_handle;

int __cxa_atexit(void (*)(void *), void *, void *)
{
  return 0;
}

int __cxa_pure_virtual()
{
	hwErrorMode();
}

// This is the code GCC generates:
//      static <type> guard;
//      if (!guard.first_byte) {
//        if (__cxa_guard_acquire (&guard)) {
//          bool flag = false;
//          try {
//            // Do initialization.
//            flag = true; __cxa_guard_release (&guard);
//            // Register variable for destruction at end of program.
//           } catch {
//          if (!flag) __cxa_guard_abort (&guard);
//         }
//      }
//
//
// We will use three values:
//     INIT_NONE = 0      => no initialisation yet
//     INIT_PENDING       => initialisation running
//     INIT_SUCCESS       => initialisation successfully done
//
// Due to the above code from gcc, we see that INIT_PENDING
// has 0 in the first byte so that _acquire is invoked (and can wait).
//
// All the INIT_* values are crafted to be easily comparable (with a single
// subcc, Z and N flags carry all information which one it is), easily
// loadable, i.e., with one machine instruction, and the first and the last
// byte are the same, so that we don't have to think about endianness wrt.
// first-byte test as mentioned above.

#define INIT_NONE    0x00000000  // =0, one instr. to load, false, byte[0] = 0
#define INIT_PENDING 0x00010000  // >0, exactly on bit set => one instr. to load, byte[0] = 0
#define INIT_SUCCESS 0xffffffff  // <0, one instr. to load, byte[0] != 0 (regardless of endianness)

int __cxa_guard_acquire (unsigned *guard)
{
  while (1) {
    unsigned old= hwEnterTrapMode();
    switch (*guard) {
    case INIT_PENDING:
      hwLeaveTrapMode(old);
      Thread::yield();
      break;

    case INIT_SUCCESS:
      hwLeaveTrapMode(old);
      return 0;

    default:
      *guard= INIT_PENDING;
      hwLeaveTrapMode(old);
      return 1;
    }
  }
}

void __cxa_guard_release (unsigned *guard)
{
  *guard= INIT_SUCCESS;
}

void __cxa_guard_abort (unsigned *guard)
{
  *guard= INIT_NONE;
}
