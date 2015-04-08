/* -*- Mode: Assembler */

/**
 * @file
 * @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Copyright 2008-2010 DLR
 *
 * @brief Code run during boot-up/reset.
 */

#include "hw_asmdef.h"

//////////////////////////////////////////////////////////////////////
// Booting needs a special section that is never relocated,
// but is executed from ROM.  It copies .text from ROM to RAM, then
// normal .text can be used.
.section HW_SEC_STARTUP

.global hwTrapResetContinue
.type   hwTrapResetContinue, #function
hwTrapResetContinue:

  // Check the leon version we are running on;
  // in case of LEON3 enable the instruction and data caches.
  rd    %psr, %g1
  srl   %g1, 24, %g1              ! psr.version
  and   %g1, 0xf, %g1
  cmp   %g1, 0x3                  ! for != 0x3 we assume leon2
  set   (CCR_ICSE|CCR_DCSE|CCR_IF|CCR_DF|CCR_IB|CCR_SN), %g1
  bne   leon2
   nop

  // LEON3: CCR needs to be set in a different ASI
leon3:
  set   CCR_OFFSET, %g2
  sta   %g1, [%g2] ASI_CCR

  // LEON2: CCR is set as part of hw_config.cpp; nothing to do here
leon2:

  /* The trap handler already did:

  flush zero
  mov   ~0, %wim
  b     hwTrapResetContinue
  mov   0,  %tbr

  */
  NOP_WAIT0    // for WIM, TBR

  /* Initialise other global registers. */
  mov   0, %g1
  sethi %hi(hwZero), %g1

  ldd   [ %g1 + %lo(hwZero) ], %g2
  ldd   [ %g1 + %lo(hwZero) ], %g4
  ldd   [ %g1 + %lo(hwZero) ], %g6

  /* Initialise PSR: set PIL to maximum */
  sethi CONST_HI(HW_INIT_PSR_SUPERVISOR | HW_BITS(HW_PSR_PIL)), %g7
  wr    %g7, CONST_LO(HW_INIT_PSR_SUPERVISOR | HW_BITS(HW_PSR_PIL)), %psr
  NOP_WAIT0        // for PSR */

  /* hwTrapTable-0x10 must be aligned by 4096, so no %lo() part
   * is needed here. */
  sethi %hi(hwTrapTable - 0x10), %g7
  wr    %g7, %tbr  // trap base register
  NOP_WAIT0        // (for TBR)

  /* Store WIM for later analysis (parameter to hwResetContinue)
   * We stored ~0 in it, so the implemented windows should be 1, the others 0. */
  rd   %wim, %g3   // WIM!, PSR.CWP!

  /* Initialise WIM: window 0 (i.e., the current one!) is invalid
   * Next SAVE will make WIM=1, which will not cause a trap. */
  mov  1, %wim
  NOP_WAIT0        // for WIM


  /* Initialise %y */
  mov  0, %y
  NOP_WAIT0        // (for Y)

  /* ** We can use %r8..%r31 now, since the window pointer (in PSR) is set. ** */

  /* Initialise all local windows: we're now in the invalid window, and after N-1
   * RESTOREs, we're where a SAVE would have brought us: in the first valid register
   * window. */
  mov   HW_CONFIG_REG_WINDOWS,%g5
.L_NextRegisterWindow:
  subcc %g5, 1, %g5

  .irp REG, 0,2,4,6
  ldd   [ %g1 + %lo(hwZero) ], %l\REG           // PSR.CWP!
  ldd   [ %g1 + %lo(hwZero) ], %i\REG
  .endr

  bnz,a .L_NextRegisterWindow
  restore                                       // WIM!

  /* Initialise system stack: we won't write to the stack yet,
   * because we will never return.  We initialise both %fp and %sp
   * as if we set the %sp in the invalid window and then had done a
   * normal save with stack shift.  Since we're already in the right
   * register window, we don't need the SAVE here -- the N-1 RESTOREs
   * have already done that. */
  set   __stackEnd, %fp
  add   %fp, -HW_FRAME_SIZEOF, %sp

  /*
   * We could call careful functions from here on that do not use any RAM (including stack).
   * So we'll do the basic hardware configuration now.
   */

  /* Initialise hardware registers: */

  /* Memory control register 1 needs special care: */
  set    HW_BASE_OCR, %g4

  sethi  %hi(hwInitMCFG1), %l0
  ld     [ %l0 + %lo(hwInitMCFG1) ], %l0
  ld     [ %g4 + HW_OCR_MCFG1 ], %l1             // read PROM width from register
  andn   %l0, HW_BITS(HW_MCFG1_PROM_WIDTH), %l0  // better safe than sorry: clear those bits here
  and    %l1, HW_BITS(HW_MCFG1_PROM_WIDTH), %l1
  or     %l0, %l1, %l0
  st     %l0, [ %g4 + HW_OCR_MCFG1 ]

#if HW_CONFIG_ARCH == HW_ARCH_LEON2_SPWRTC
  /* MCFG3 needs special attention on LEON2 SPWRTC, since the PROM EDAC enable
   * bit is initialised from the PIO[2] DIP switch setting, and we do not want
   * to change that bit. If RAM EDAC is enabled, we set that bit here (and there
   * is another config setting in FT AHB RAM register, which has as more trivial
   * initialisation (see hw_config.cpp). */

#if HW_CONFIG_ENABLE_EDAC
  ld     [ %g4 + HW_OCR_MCFG3 ], %l0
  or     %l0, (1 << 9), %l0                      // set RAM EDAC enable bit
  st     %l0, [ %g4 + HW_OCR_MCFG3 ]
#endif

#endif

  /* Other hardware registers, trivial boot-up initialisation.  Anything more involved
   * must be done later. */
  set    hwInitConfigRegVal, %l0
.L_NextHardwardReg:
  ldd    [ %l0 ], %l2                           // This is 8-byte aligned
  add    %l0, 8, %l0
  orcc   %l2, %l2, zero
  bnz,a  .L_NextHardwardReg
  st     %l3, [ %l2 ]

  /* Current state:
   *   - still in supervisor mode
   *   - all trap still disabled (ET=0)
   *   - interrupt controller is programmed
   *   - RAM incl. stack is accessible now
   *   - Still no variables usable, because RAM is uninitialised and must be,
   *     copied from ROM so that's the first thing the C reset handler must do.
   *
   * We're ready to call a C routine.  There must be no trap
   * in this state, or the CPU will enter error mode.
   *
   * Now call the C reset handler which does the rest.
   *
   *  %o0: param 1: powerOnPSR (already in place)
   *  %o1: param 2: powerOnWIM (already in place)
   *
   * Reset %o7 to point to failure routine in case the function is
   * broken and returns.
   */

  /* Initialise %fsr */
#if HW_CONFIG_HAVE_FLOAT
  ld   [ %g1 + %lo(hwZero) ], %fsr
  NOP_WAIT0        // (for FSR)

  .irp REG, 0,2,4,6,8, 10,12,14,16,18, 20,22,24,26,28, 30
  ldd  [ %g1 + %lo(hwZero) ], %f\REG
  .endr
#endif

  /* Possibly Write Unprotect: grmon has no knowledge of these,
   * so it won't reset them.  Let's have them in a sane state so we can
   * copy read-only stuff from ROM to RAM. */
#if HW_CONFIG_HAVE_EXT_WRITE_PROTECT
  st    %g0, [ %g4 + HW_OCR_WRITE_PROTECT_1_START ]
  st    %g0, [ %g4 + HW_OCR_WRITE_PROTECT_1_END   ]
#endif

#if !HW_CONFIG_RAMIMAGE
#if HW_CONFIG_CODE_IN_RAM
  /* Copy .text from ROM to RAM: */
  set   __textStart,    %o0
  set   __textROMStart, %o1
  sethi %hi(__textEnd), %o2
  call  memCopy64
  or    %o2, %lo(__textEnd), %o2
#endif

  /* Maybe copy .data from ROM to RAM (not done when we're in simulator): */
  set   __dataStart,    %o0
  set   __dataROMStart, %o1
  subcc %o0, %o1, zero
  be    .L_inSimulator
  sethi %hi(__dataEnd), %o2

  call  memCopy64
  or    %o2, %lo(__dataEnd), %o2

  /* Copy data from ROM to SDRAM: */
  set   __SDRAMDataStart,    %o0
  set   __SDRAMDataROMStart, %o1
  sethi %hi(__SDRAMDataEnd), %o2
  call  memCopy64
  or    %o2, %lo(__SDRAMDataEnd), %o2
#endif //HW_CONFIG_RAMIMAGE


  /* Clear bss: */
  set   __bssStart,    %o0
  sethi %hi(__bssEnd), %o1
  call  memClear64                     // first call to code in RAM (normal .text section)
  or    %o1, %lo(__bssEnd), %o1

  /* Clear stack (in the presence of EDAC support, better clear all used RAM): */
  set   __stackStart,    %o0
  sethi %hi(__stackEnd), %o1
  call  memClear64
  or    %o1, %lo(__stackEnd), %o1

.L_inSimulator:

  // Possibly Write Protect:
  //NOTE: DEACTIVATED DUE TO PROBLEMS WHEN THE WHOLE IMAGES IS LOADED IN RAM BY GRLIB
/*#if HW_CONFIG_HAVE_EXT_WRITE_PROTECT && (HW_CONFIG_RODATA_IN_RAM || HW_CONFIG_CODE_IN_RAM) && !HW_CONFIG_RAMIMAGE
  set   __writeProtectStart + \
        HW_BITS(HW_WRITE_PROTECT_BP), %o0

  st    %o0, [ %g4 + HW_OCR_WRITE_PROTECT_1_START ]

  set   __writeProtectEnd + \
        HW_BITS(HW_WRITE_PROTECT_US) + \
        HW_BITS(HW_WRITE_PROTECT_SU) - 4, %o0

  st    %o0, [ %g4 + HW_OCR_WRITE_PROTECT_1_END ]
#endif*/

  // Read PSR to O0 (first param of hwResetContinue), and then enable traps.
  rd    %psr, %o0
  wr    %o0, HW_BITS(HW_PSR_ET), %psr  // enable traps
  NOP_WAIT0

  // Go to C code, we're ready!

  // Keep only PS, PIL, VER, and IMPL flags, so this is suitable
  // for both checking the hardware architecture as well as for
  // invoking hwLeaveTrapMode().
  call  hwResetContinue
  mov   %g3, %o1


.global memCopy64                // Needed in ROM to copy .text from ROM to RAM
.type   memCopy64, #function
memCopy64:
  // o0:   dst
  // o1:   src
  // o2:   dst_end
  // o4/5: data
  subcc  %o0, %o1, zero           // src == dst? => raus
  be     .L_memCopy64End
  subcc  %o2, %o0, %o2

  bz     .L_memCopy64End          // len == 0? => raus
  add    %o0, 8, %o0

.L_memCopy64NextWord:
  ldd    [ %o1 + %o2 ], %o4
  subcc  %o2, 8, %o2
  bcc    .L_memCopy64NextWord
  std    %o4, [ %o0 + %o2 ]

.L_memCopy64End:
  retl
  nop


//////////////////////////////////////////////////////////////////////
// Normal Text Section
.section HW_SEC_TEXT

.global memClear64
.type   memClear64, #function
memClear64:
  // o0:   dst
  // o1:   dst_end
  // g0/1: 0
  mov    zero, %g1

  b      .L_MemClear64EnterLoop
  subcc  %o1, %o0, %o1

.L_MemClear64NextWord:
  std    zero, [ %o0 + %o1 ]
.L_MemClear64EnterLoop:
  bgu    .L_MemClear64NextWord
  subcc  %o1, 8, %o1

  retl
  nop


//////////////////////////////////////////////////////////////////////
// DATA:
.section HW_SEC_ROM_DATA

  .align 8
.global hwZero
hwZero:
  .long 0
  .long 0

