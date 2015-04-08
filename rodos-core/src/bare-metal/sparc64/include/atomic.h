/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief SPARC V9 Atomic instructions.
 */

#ifndef _SPARC64_ATOMIC_H
#define _SPARC64_ATOMIC_H

#include <stdint.h>

#define atomic_init(i)              {(i)}
#define atomic_get(v)               ((v)->counter)
#define atomic_set(v, i)            ((v)->counter = (i))

typedef struct {
  int64_t counter;
} atomic_t;

/**
 * @brief Atomic compare and exchange.
 * 'cas' compares v with o(ld), if identical it stores n(ew) in v.
 * To put it differently, the register is compared with a memory location, if
 * they are equal, the memory content is swapped with the contents of the
 * second register.
 *
 * @param v volatile memory
 * @param o old value
 * @param n new value
 *
 * @retval old value; test if successful by comparing old with return value
 */
static inline int64_t atomic_cmpxchg(atomic_t *v, int64_t o, int64_t n) {

  asm volatile("casx [%2], %3, %0\n"
      : "=&r" (n)
      : "0" (n), "r" (&v->counter), "r" (o)
      : "memory");

  return n;
}

#endif /* _SPARC64_ATOMIC_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
