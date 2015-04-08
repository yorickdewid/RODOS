/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief SPARCv9 processor state registers.
 */

#ifndef _SPARC64_PSTATE_H
#define _SPARC64_PSTATE_H

/* SPARCv9 WSTATE (window state) Register.
 * Which one of the eight different window spill and fill handlers is taken
 * depends on the contents of the %wstate register.
 *
 * 5       3 2       0
 * +--------+--------+
 * | OTHER  | NORMAL |
 * +--------+--------+
 *
 * If OTHERWIN=0 then the wstate.normal bits are inserted into TT[TL]
 * Otherwise, the wstate.other bits are inserted into TT[TL].
 */

#define WSTATE_O(i)     (i << 3)
#define WSTATE_N(i)     (i << 0)

/* SPARCv9 PSTATE Register with SpitFire extensions (replaces the PSR).
 *
 *    11   10    9     8   7  6   5     4     3     2     1   0
 *  +----+----+-----+-----+----+-----+-----+----+------+----+----+
 *  | IG | MG | CLE | TLE | MM | RED | PEF | AM | PRIV | IE | AG |
 *  +----+----+-----+-----+----+-----+-----+----+------+----+----+
 */

#define PSTATE_IG         0x0000000000000800 /* IG - Interrupt Globals    */
#define PSTATE_MG         0x0000000000000400 /* MG - MMU Globals          */
#define PSTATE_CLE        0x0000000000000200 /* Current Little Endian     */
#define PSTATE_TLE        0x0000000000000100 /* Traps Little Endian       */
#define PSTATE_MM         0x00000000000000c0 /* Memory Model              */
#define PSTATE_TSO        0x0000000000000000 /* MM: Total Store Order     */
#define PSTATE_PSO        0x0000000000000040 /* MM: Partial Store Order   */
#define PSTATE_RMO        0x0000000000000080 /* MM: Relaxe dMemory Order  */
#define PSTATE_RED        0x0000000000000020 /* Reset Error Debug state   */
#define PSTATE_PEF        0x0000000000000010 /* Enable Floating Point     */
#define PSTATE_AM         0x0000000000000008 /* 32-bit Address Masking    */
#define PSTATE_PRIV       0x0000000000000004 /* Privileged mode           */
#define PSTATE_IE         0x0000000000000002 /* Interrupt enable          */
#define PSTATE_AG         0x0000000000000001 /* AG - Alternate Globals    */


/* SPARCv9 TSTATE Register
 *
 *  63   43 42  40 39   32 31   24 23    20 19       8 7      5 4     0
 * +-------+------+-------+-------+--------+----------+--------+-------+
 * |   -   |  GL  |  CCR  |  ASI  |  %pil  |  PSTATE  |   -    |  CWP  |
 * +-------+------+-------+-------+--------+----------+--------+-------+
 */

#define TSTATE_GL         0x0000070000000000 /* Global reg Level          */
#define TSTATE_CCR        0x000000ff00000000 /* Condition Code Register   */
#define TSTATE_XCC        0x000000f000000000 /* 64-bit Condition Codes    */
#define TSTATE_XNEG       0x0000008000000000 /* %xcc Negative             */
#define TSTATE_XZERO      0x0000004000000000 /* %xcc Zero                 */
#define TSTATE_XOVFL      0x0000002000000000 /* %xcc Overflow             */
#define TSTATE_XCARRY     0x0000001000000000 /* %xcc Carry                */
#define TSTATE_ICC        0x0000000f00000000 /* 32-bit Condition Codes    */
#define TSTATE_INEG       0x0000000800000000 /* %icc Negative             */
#define TSTATE_IZERO      0x0000000400000000 /* %icc Zero                 */
#define TSTATE_IOVFL      0x0000000200000000 /* %icc Overflow             */
#define TSTATE_ICARRY     0x0000000100000000 /* %icc Carry                */
#define TSTATE_ASI        0x00000000ff000000 /* Address Space ID          */
#define TSTATE_PIL        0x0000000000f00000 /* %pil                      */
#define TSTATE_PSTATE     0x00000000000fff00 /* PSTATE                    */
#define TSTATE_IG         0x0000000000080000 /* IG - Interrupt Globals    */
#define TSTATE_MG         0x0000000000040000 /* MG - MMU Globals          */
#define TSTATE_CLE        0x0000000000020000 /* Current Little Endian     */
#define TSTATE_TLE        0x0000000000010000 /* Traps Little Endian       */
#define TSTATE_MM         0x000000000000c000 /* Memory Model              */
#define TSTATE_TSO        0x0000000000000000 /* MM: Total Store Order     */
#define TSTATE_PSO        0x0000000000004000 /* MM: Partial Store Order   */
#define TSTATE_RMO        0x0000000000008000 /* MM: Relaxe dMemory Order  */
#define TSTATE_RED        0x0000000000002000 /* Reset Error Debug state   */
#define TSTATE_PEF        0x0000000000001000 /* Enable Floating Point     */
#define TSTATE_AM         0x0000000000000800 /* 32-bit Address Masking    */
#define TSTATE_PRIV       0x0000000000000400 /* Privileged mode           */
#define TSTATE_IE         0x0000000000000200 /* Interrupt enable          */
#define TSTATE_AG         0x0000000000000100 /* AG - Alternate Globals    */
#define TSTATE_SYSCALL    0x0000000000000020 /* in Syscall trap           */
#define TSTATE_CWP        0x000000000000001f /* Current Windoe Pointer    */

#define TSTATE_SHIFT_GL       40
#define TSTATE_SHIFT_CCR      32
#define TSTATE_SHIFT_ASI      24
#define TSTATE_SHIFT_PIL      20
#define TSTATE_SHIFT_PSTATE   8
#define TSTATE_SHIFT_CWP      0


/* Floating-Point Registers State Register.
 *
 *  63     3    2       1      0
 * +--------+-------+------+------+
 * |   -    |  FEF  |  DU  |  DL  |
 * +--------+-------+------+------+
 */
#define FPRS_FEF          (1 << 2) /* FEF fpu enable                      */
#define FPRS_DU           (1 << 1) /* DU dirty upper                      */
#define FPRS_DL           (1 << 0) /* DL dirty lower                      */


/* SPARCv9 VER (version) Register.
 *
 *  63   48 47  32 31  24 23  16 15    8 7    5 4      0
 * +-------+------+------+------+-------+------+--------+
 * | MANUF | IMPL | MASK |  -   | MAXTL |  -   | MAXWIN |
 * +-------+------+------+------+-------+------+--------+
 */
#define VERS_MANUF        0xffff000000000000 /* MANUFacturer              */
#define VERS_IMPL         0x0000ffff00000000 /* IMPLementation            */
#define VERS_MASK         0x00000000ff000000 /* MASK set revision         */
#define VERS_MAXTL        0x000000000000ff00 /* MAXTL max trap level      */
#define VERS_MAXWIN       0x000000000000001f /* MAXWIN max regwin index   */

#endif /* _SPARC64_PSTATE_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */