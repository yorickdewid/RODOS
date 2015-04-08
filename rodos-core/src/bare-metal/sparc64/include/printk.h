/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Kernel printf for sparc64, also makes putchar, puts visible.
 */

#ifndef _SPARC64_PRINTK_H
#define _SPARC64_PRINTK_H

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int puts(const char *s);
extern int putchar(int c);
extern int printk(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* _SPARC64_PRINTK_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
