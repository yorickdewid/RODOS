/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Load-Store-Unit Control Register.
 */

#ifndef _SPARC64_LSU_H
#define _SPARC64_LSU_H

/**
 * @defgroup hwLSU Load-Store-Unit control.
 */
/**@{*/

/* Load-Store-Unit Control Register.
 * NOTE:  Since UltraSPARC III this is called
 *        Data Cache Unit Control Register (DCUCR)
 */
#define LSU_CONTROL_PM 0x000001fe00000000 /* phys-watchpoint byte mask    */
#define LSU_CONTROL_VM 0x00000001fe000000 /* virt-watchpoint byte mask    */
#define LSU_CONTROL_PR 0x0000000001000000 /* phys-rd watchpoint enable    */
#define LSU_CONTROL_PW 0x0000000000800000 /* phys-wr watchpoint enable    */
#define LSU_CONTROL_VR 0x0000000000400000 /* virt-rd watchpoint enable    */
#define LSU_CONTROL_VW 0x0000000000200000 /* virt-wr watchpoint enable    */
#define LSU_CONTROL_FM 0x00000000000ffff0 /* parity mask enables          */
#define LSU_CONTROL_DM 0x0000000000000008 /* data d-MMU enable            */
#define LSU_CONTROL_IM 0x0000000000000004 /* instruction i-MMU enable     */
#define LSU_CONTROL_DC 0x0000000000000002 /* data cache enable            */
#define LSU_CONTROL_IC 0x0000000000000001 /* instruction cache enable.    */

/**@}*/

#endif /* _SPARC64_LSU_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
