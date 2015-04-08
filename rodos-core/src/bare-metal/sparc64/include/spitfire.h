/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief SPARCv9 Spitfire MMU operations and registers.
 */

#ifndef _SPARC64_SPITFIRE_H
#define _SPARC64_SPITFIRE_H

/* There is a distinct and unique copy of these registers for each TLB.
 * These addresses are accessible via ASI_DMMU and ASI_IMMU.
 */
#define TSB_TAG_TARGET      0x0000000000000000
#define TLB_SFSR            0x0000000000000018
#define TSB_REG             0x0000000000000028
#define TLB_TAG_ACCESS      0x0000000000000030
#define VIRT_WATCHPOINT     0x0000000000000038
#define PHYS_WATCHPOINT     0x0000000000000040 /* ... all architectures */
#define TSB_EXTENSION_P     0x0000000000000048 /* since UltraSPARC-III  */
#define TSB_EXTENSION_S     0x0000000000000050 /* since UltraSPARC-III  */
#define TSB_EXTENSION_N     0x0000000000000058 /* since UltraSPARC-III  */
#define TLB_TAG_ACCESS_EXT  0x0000000000000060 /* since UltraSPARC-III+ */

/* These registers only exist as one entity.
 * These addresses are accessible via ASI_DMMU only.
 */
#define PRIMARY_CONTEXT     0x0000000000000008
#define SECONDARY_CONTEXT   0x0000000000000010
#define DMMU_SFAR           0x0000000000000020
#define VIRT_WATCHPOINT     0x0000000000000038
#define PHYS_WATCHPOINT     0x0000000000000040

/* These are valid for UltraSPARC I and II */
#define MMU_VIRT_BITS       44
#define MMU_PHYS_BITS       41

#endif /* _SPARC64_SPITFIRE_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */