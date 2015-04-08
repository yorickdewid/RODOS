/**
 * @author Andre Bartke <dev@bartke.cc>
 * 
 * @brief Atomic opcode tests.
 *
 * Note: These tests do not run in parallel, their purpose is to check if the
 * implemented atomic opcodes work on atomics as expected.
 */

#include <rodos.h>
#include <atomic.h>

#define COMPARE(name, i, j)                                 \
  if (i == j)                                               \
    PRINTF("OK\n");                                         \
  else                                                      \
    PRINTF("ERROR: %s: expected 0x%02x but was 0x%02x\n",   \
        #name, (unsigned int) i, (unsigned int) j);

class atomictest : public Thread {
  public:
    void run() {
      atomic_t m = atomic_init(0);
      atomic_t *v = &m;
      int retval;

      // cmpxchg shall succeed, set atomic from 0 to 1
      retval = atomic_cmpxchg(v, 0, 1);
      COMPARE(atomic_cmpxchg(v, 0, 1), 1, atomic_get(v));
      COMPARE(return_value, retval, 0);

      // cmpxchg comparison shall fail, keep atomic value 1
      retval = atomic_cmpxchg(v, 2, 3);
      COMPARE(atomic_cmpxchg(v, 2, 3), 1, atomic_get(v));
      COMPARE(return_value, retval, 1);

      PRINTF("\nDONE\n");
    }
} atomictest;

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
