/**
 * @file atomic.h
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief PowerPC Atomic instructions.
 */

#ifndef _POWERPC_ATOMIC_H
#define _POWERPC_ATOMIC_H

#include <stdint.h>

#define atomic_init(i)              {(i)}
#define atomic_get(v)               ((v)->counter)
#define atomic_set(v, i)            ((v)->counter = (i))

typedef struct {
  int32_t counter;
} atomic_t;

/**
 * @brief Atomic compare and exchange.
 * There are two semaphore-synchronization instructions. These can be used
 * to implement test and set, compare and swap and exchange memory.
 *
 * @param v volatile memory
 * @param o old value
 * @param n new value
 *
 * @retval old value; test if successful by comparing old with return value
 */
static inline int32_t atomic_cmpxchg(atomic_t *v, int32_t o, int32_t n) {

  int32_t ret;

  asm volatile(
      "1: lwarx   %0, 0, %2\n"  /* load word and reserve */
      "   cmpw    0, %0, %3\n"  /* compare value and new */
      "   bne-    2f\n"         /* exit if not equal */
      "   sync\n"               /* flush instruction pipeline */
      "   stwcx.  %4, 0, %2\n"  /* store word if still reserved */
      "   bne-    1b\n"         /* try again if we lost the reservation */
      "2: \n"
      : "=&r" (ret), "+m" (*v)
      : "r" (&v->counter), "r" (o), "r" (n)
      : "cc", "memory");        // tell gcc what happened

  return ret;
}

#endif /* _POWERPC_ATOMIC_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
