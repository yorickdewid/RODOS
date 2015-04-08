/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Context switching globals.
 */

#include <context.h>

/** ctx=0 is reserved for supervisor, start with 1.
 *
 * FIXME:
 * Currently this value is just incremented as we instantiate threads because
 * we will never reach 8192 number of contexts and consequently do not
 * overflow.
 * It is however a design flaw if the system grows and potentially reach this
 * limit.
 */
uint16_t ctx_number=1;

/** Supervisor context. */
scontext_t __sctx = {.id=0};

/** Current context pointer. */
context_t *__cctx = (context_t*) &__sctx;

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
