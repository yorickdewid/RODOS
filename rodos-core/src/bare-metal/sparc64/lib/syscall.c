/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief System call wrapper function.
 */

#include <syscall.h>

uint64_t _syscall6(
    uint8_t number,
    uint64_t o0,
    uint64_t o1,
    uint64_t o2,
    uint64_t o3,
    uint64_t o4,
    uint64_t o5) {

  /* copy arguments */
  register uint64_t arg0 asm("o0") = o0;
  register uint64_t arg1 asm("o1") = o1;
  register uint64_t arg2 asm("o2") = o2;
  register uint64_t arg3 asm("o3") = o3;
  register uint64_t arg4 asm("o4") = o4;
  register uint64_t arg5 asm("o5") = o5;

  /* ta n, issues trap 0x100 + n */
  asm volatile(
      "mov  %7, %%g1\n"
      "ta   %8\n"
      : "=r" (arg0)
      : "r" (arg0),
        "r" (arg1),
        "r" (arg2),
        "r" (arg3),
        "r" (arg4),
        "r" (arg5),
        "r" (number),
        "i" (SYS_syscall)
      : "memory");

  return arg0;
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
