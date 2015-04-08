/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Kernel printf for sparc64, also provide write, putchar functions.
 */

#ifndef _SPARC64_ASM_H
#define _SPARC64_ASM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint64_t prom_cif; /**< entry point for PROM calls */
extern uint64_t prom_tba; /**< backup of the PROM tba */

/** Switch to the kernel trap table. */
extern void setup_traptable(void);

/** Kernel panic printf, freezes after printing. */
extern void panic(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* _SPARC64_ASM_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
