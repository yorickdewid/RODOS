/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Typedefs for some special variable types.
 */

#ifndef _SPARC64_TYPES_H
#define _SPARC64_TYPES_H

#ifndef __ASM__

/* register size */
typedef unsigned long                     register_t;

/* function pointer */
typedef void (*funcptr_t)();

#endif /* __ASM__ */
#endif /* _SPARC64_TYPES_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
