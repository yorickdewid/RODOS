/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Definitions and data structures for context switching.
 */

#ifndef _SPARC64_CONTEXT_H
#define _SPARC64_CONTEXT_H

#include <stdint.h>
#include <param.h>

/**
 * @brief Supervisor context.
 * The supervisor context can start with scratch registers, so the only
 * interesting property is the context id for identification.
 */
typedef struct {
  uint64_t id; /**< context id */
} scontext_t;


/**
 * @brief Thread Context.
 */
typedef struct {
  uint64_t id; /**< context id */

  /* -- starting from here: to be preserved on involuntary ctx switch -- */

  uint64_t g1; /* 1-7: globals */
  uint64_t g2;
  uint64_t g3;
  uint64_t g4;
  uint64_t g5;
  uint64_t g6;
  uint64_t g7;

  uint64_t o0; /* 8-15: outs */
  uint64_t o1;
  uint64_t o2;
  uint64_t o3;
  uint64_t o4;
  uint64_t o5;

  /* -- starting from here: to be preserved accross function calls -- */

  uint64_t sp; /**< (%o6) stack pointer */
  uint64_t o7; /**< subroutine return address */

  uint64_t l0; /* 16-23: locals */
  uint64_t l1;
  uint64_t l2;
  uint64_t l3;
  uint64_t l4;
  uint64_t l5;
  uint64_t l6;
  uint64_t l7;

  uint64_t i0; /* 24-31: ins */
  uint64_t i1;
  uint64_t i2;
  uint64_t i3;
  uint64_t i4;
  uint64_t i5;
  uint64_t fp; /**< (%i6) frame pointer */
  uint64_t i7; /**< function return address */

  uint64_t pc; /**< 32: program counter*/

  /* -- Control registers, important on involuntary context switch -- */
  uint64_t ccr; /**< 33: condition code register */
  uint64_t y; /**< 34: y register (even if deprecated, gcc may use it) */
} context_t;

extern uint16_t ctx_number;
extern context_t *__cctx;
extern scontext_t __sctx;

/**
 * @brief Check if executed from supervisor.
 *
 * @return true if this is executed from supervisor mode, otherwise false.
 */
static inline int is_supervisor(void) {
  return (__cctx->id == 0);
}

#ifdef __cplusplus
extern "C" {
#endif

extern void __attribute__ ((__noreturn__)) __asmSaveContext(void);
extern void __attribute__ ((__noreturn__)) __asmSaveContextAndCallScheduler(void);
extern void __attribute__ ((__noreturn__)) __asmSwitchToContext(long* context);

#ifdef __cplusplus
}
#endif

#endif /* _SPARC64_CONTEXT_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
