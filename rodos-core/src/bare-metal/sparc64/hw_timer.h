/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief RODOS interfaces, time keeping and tick interrupts.
 */

#ifndef _SPARC64_TIMER_H
#define _SPARC64_TIMER_H

#include <hw_datatypes.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void tick_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* _SPARC64_TIMER_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
