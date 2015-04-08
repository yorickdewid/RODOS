/* -*- Mode: Assembler */

/**
 * @file
 * @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Copyright 2008-2010 DLR
 *
 * @brief The trap table, including the reset vector.
 */

#include "hw_asmdef.h"

/* Traps are dispatched to C via hwSysTrapTrampoline. */
#define HANDLE_SYS_TRAP(_N)                     \
  rd    %psr, %l0                             ; \
  sethi %hi(hwTrapHandler##_N), %l4           ; \
  b     hwSysTrapTrampoline                   ; \
  or    %l4, %lo(hwTrapHandler##_N), %l4

/* New user traps are part of the low-level system, because Rodos simply
 * invokes system functions if necessary, not using a dedicated call gate
 * into the kernel.  So for the sake of simplicity, there is no gate to
 * C code for them.  Thus we simply cause a system panic if one is invoked. */
#define HANDLE_USER_TRAP(_N)                  \
  rd    %psr, %l0                             ; \
  sethi %hi(hwTrapHandler##_N), %l4           ; \
  b     hwSysTrapTrampoline                   ; \
  or    %l4, %lo(hwTrapHandler##_N), %l4


//////////////////////////////////////////////////////////////////////
// Hardware reset vector: this has a special section so we can
// place it at 0x0
.section HW_SEC_RESET

.global hwStartUp
.type   hwStartUp, #function
hwStartUp:
hwTrapReset:
hwTrap00:
  /* When we reach this point, we assume that:
   *
   *    PC     = 0x0    we're here
   *    nPC    = 0x4    next instruction is the following one
   *    PSR.ET = 0      traps disabled
   *    PSR.S  = 1      in supervisor mode
   *    CCR    = 0      cache control
   *
   * Several other hardware registers are in a state such that ROM code executes
   * for a while without trapping.  E.g. the timer prescaler produces very slow
   * ticks and the watchdog is fully charged.
   *
   * We now start to initialise everything to a sane state.
   */
  flush zero		             // start with a cache flush
  mov   ~0, %wim                 // set all register window invalid
  b     hwTrapResetContinue
  mov   0,  %tbr

//////////////////////////////////////////////////////////////////////
// Trap table: starts with vector for trap 0x01, so that it may
// directly follow the reset section.
// Note: this must be aligned at 16 MOD 4096, otherwise we cannot
// load the TBR register, which will be loaded with hwTrapTable - 0x10.
// The linker script will enforce this alignment.
.section HW_SEC_TRAP

.global hwTrapTable
hwTrapTable:

hwTrapInstructionAccessException:
hwTrap01: HANDLE_SYS_TRAP(0x01)

hwTrapIllegalInstruction:
hwTrap02: HANDLE_SYS_TRAP(0x02)

hwTrapPrivilegedInstruction:
hwTrap03: HANDLE_SYS_TRAP(0x03)

hwTrapFPDisabled:
hwTrap04: HANDLE_SYS_TRAP(0x04)

hwTrapWindowOverflow:
hwTrap05:
  rd    %wim, %l3                          // get current WIM
  mov   %g1, %l7                           // need one register that stays intact during SAVE
  b     hwTrapWindowOverflowContinue
  srl   %l3, 1, %g1                        // start rotating

hwTrapWindowUnderflow:
hwTrap06:
  rd    %wim, %l3
  sll   %l3, 1, %l7
  b     hwTrapWindowUnderflowContinue
  srl   %l3, HW_CONFIG_REG_WINDOWS - 1, %l3

hwTrapMemAddressNotAligned:
hwTrap07: HANDLE_SYS_TRAP(0x07)

hwTrapFPException:
hwTrap08: HANDLE_SYS_TRAP(0x08)

hwTrapDataAccessException:
hwTrap09: HANDLE_SYS_TRAP(0x09)

hwTrapTagOverflow:
hwTrap0A: HANDLE_SYS_TRAP(0x0A)

hwTrapWatchpointDetected:
hwTrap0B: HANDLE_SYS_TRAP(0x0B)

hwTrap0C: HANDLE_SYS_TRAP(0x0C)
hwTrap0D: HANDLE_SYS_TRAP(0x0D)
hwTrap0E: HANDLE_SYS_TRAP(0x0E)
hwTrap0F: HANDLE_SYS_TRAP(0x0F)
hwTrap10: HANDLE_SYS_TRAP(0x10)

hwTrapAHBError:
hwTrapInterrupt1:
hwTrap11: HANDLE_SYS_TRAP(0x11)

hwTrapUART2:
hwTrapInterrupt2:
hwTrap12: HANDLE_SYS_TRAP(0x12)

hwTrapUART1:
hwTrapInterrupt3:
hwTrap13: HANDLE_SYS_TRAP(0x13)

hwTrapParport0:
hwTrapInterrupt4:
hwTrap14: HANDLE_SYS_TRAP(0x14)

hwTrapParport1:
hwTrapInterrupt5:
hwTrap15: HANDLE_SYS_TRAP(0x15)

hwTrapParport2:
hwTrapInterrupt6:
hwTrap16: HANDLE_SYS_TRAP(0x16)

hwTrapParport3:
hwTrapInterrupt7:
hwTrap17: HANDLE_SYS_TRAP(0x17)

hwTrapPreempt:
hwTrapTimer1:
hwTrapInterrupt8:
hwTrap18:
  b     hwTrapSaveContextContinue
  rd    %psr, %l0
  nop
  nop

hwTrapTicker:
hwTrapTimer2:
hwTrapInterrupt9:
hwTrap19: HANDLE_SYS_TRAP(0x19)

hwTrapSecondInterruptController:
hwTrapInterruptA:
hwTrap1A: HANDLE_SYS_TRAP(0x1A)

hwTrapDSUTraceBuffer:
hwTrapInterruptB:
hwTrap1B: HANDLE_SYS_TRAP(0x1B)

hwTrapEthernetMAC:
hwTrapInterruptC:
hwTrap1C: HANDLE_SYS_TRAP(0x1C)

hwTrapInterruptD:
hwTrap1D: HANDLE_SYS_TRAP(0x1D)

hwTrapPCI:
hwTrapInterruptE:
hwTrap1E: HANDLE_SYS_TRAP(0x1E)

hwTrapInterruptF:
hwTrap1F: HANDLE_SYS_TRAP(0x1F)

hwTrapRRegisterAccessError:
hwTrap20: HANDLE_SYS_TRAP(0x20)

hwTrapInstructionAccessError:
hwTrap21: HANDLE_SYS_TRAP(0x21)

hwTrap22: HANDLE_SYS_TRAP(0x22)
hwTrap23: HANDLE_SYS_TRAP(0x23)

hwTrapCPDisabled:
hwTrap24: HANDLE_SYS_TRAP(0x24)

hwTrapUnimplementedFlush:
hwTrap25: HANDLE_SYS_TRAP(0x25)

hwTrap26: HANDLE_SYS_TRAP(0x26)
hwTrap27: HANDLE_SYS_TRAP(0x27)

hwTrapCPException:
hwTrap28: HANDLE_SYS_TRAP(0x28)

hwTrapDataAccessError:
hwTrap29: HANDLE_SYS_TRAP(0x29)

hwTrapDivisionByZero:
hwTrap2A: HANDLE_SYS_TRAP(0x2A)

hwTrapDataStoreError:
hwTrap2B: HANDLE_SYS_TRAP(0x2B)

hwTrapDataAccessMMUMiss:
hwTrap2C: HANDLE_SYS_TRAP(0x2C)

hwTrap2D: HANDLE_SYS_TRAP(0x2D)
hwTrap2E: HANDLE_SYS_TRAP(0x2E)
hwTrap2F: HANDLE_SYS_TRAP(0x2F)
hwTrap30: HANDLE_SYS_TRAP(0x30)
hwTrap31: HANDLE_SYS_TRAP(0x31)
hwTrap32: HANDLE_SYS_TRAP(0x32)
hwTrap33: HANDLE_SYS_TRAP(0x33)
hwTrap34: HANDLE_SYS_TRAP(0x34)
hwTrap35: HANDLE_SYS_TRAP(0x35)
hwTrap36: HANDLE_SYS_TRAP(0x36)
hwTrap37: HANDLE_SYS_TRAP(0x37)
hwTrap38: HANDLE_SYS_TRAP(0x38)
hwTrap39: HANDLE_SYS_TRAP(0x39)
hwTrap3A: HANDLE_SYS_TRAP(0x3A)
hwTrap3B: HANDLE_SYS_TRAP(0x3B)

hwTrapInstructionAccessMMUMiss:
hwTrap3C: HANDLE_SYS_TRAP(0x3C)

hwTrap3D: HANDLE_SYS_TRAP(0x3D)
hwTrap3E: HANDLE_SYS_TRAP(0x3E)
hwTrap3F: HANDLE_SYS_TRAP(0x3F)
hwTrap40: HANDLE_SYS_TRAP(0x40)
hwTrap41: HANDLE_SYS_TRAP(0x41)
hwTrap42: HANDLE_SYS_TRAP(0x42)
hwTrap43: HANDLE_SYS_TRAP(0x43)
hwTrap44: HANDLE_SYS_TRAP(0x44)
hwTrap45: HANDLE_SYS_TRAP(0x45)
hwTrap46: HANDLE_SYS_TRAP(0x46)
hwTrap47: HANDLE_SYS_TRAP(0x47)
hwTrap48: HANDLE_SYS_TRAP(0x48)
hwTrap49: HANDLE_SYS_TRAP(0x49)
hwTrap4A: HANDLE_SYS_TRAP(0x4A)
hwTrap4B: HANDLE_SYS_TRAP(0x4B)
hwTrap4C: HANDLE_SYS_TRAP(0x4C)
hwTrap4D: HANDLE_SYS_TRAP(0x4D)
hwTrap4E: HANDLE_SYS_TRAP(0x4E)
hwTrap4F: HANDLE_SYS_TRAP(0x4F)
hwTrap50: HANDLE_SYS_TRAP(0x50)
hwTrap51: HANDLE_SYS_TRAP(0x51)
hwTrap52: HANDLE_SYS_TRAP(0x52)
hwTrap53: HANDLE_SYS_TRAP(0x53)
hwTrap54: HANDLE_SYS_TRAP(0x54)
hwTrap55: HANDLE_SYS_TRAP(0x55)
hwTrap56: HANDLE_SYS_TRAP(0x56)
hwTrap57: HANDLE_SYS_TRAP(0x57)
hwTrap58: HANDLE_SYS_TRAP(0x58)
hwTrap59: HANDLE_SYS_TRAP(0x59)
hwTrap5A: HANDLE_SYS_TRAP(0x5A)
hwTrap5B: HANDLE_SYS_TRAP(0x5B)
hwTrap5C: HANDLE_SYS_TRAP(0x5C)
hwTrap5D: HANDLE_SYS_TRAP(0x5D)
hwTrap5E: HANDLE_SYS_TRAP(0x5E)
hwTrap5F: HANDLE_SYS_TRAP(0x5F)
hwTrap60: HANDLE_SYS_TRAP(0x60)
hwTrap61: HANDLE_SYS_TRAP(0x61)
hwTrap62: HANDLE_SYS_TRAP(0x62)
hwTrap63: HANDLE_SYS_TRAP(0x63)
hwTrap64: HANDLE_SYS_TRAP(0x64)
hwTrap65: HANDLE_SYS_TRAP(0x65)
hwTrap66: HANDLE_SYS_TRAP(0x66)
hwTrap67: HANDLE_SYS_TRAP(0x67)
hwTrap68: HANDLE_SYS_TRAP(0x68)
hwTrap69: HANDLE_SYS_TRAP(0x69)
hwTrap6A: HANDLE_SYS_TRAP(0x6A)
hwTrap6B: HANDLE_SYS_TRAP(0x6B)
hwTrap6C: HANDLE_SYS_TRAP(0x6C)
hwTrap6D: HANDLE_SYS_TRAP(0x6D)
hwTrap6E: HANDLE_SYS_TRAP(0x6E)
hwTrap6F: HANDLE_SYS_TRAP(0x6F)
hwTrap70: HANDLE_SYS_TRAP(0x70)
hwTrap71: HANDLE_SYS_TRAP(0x71)
hwTrap72: HANDLE_SYS_TRAP(0x72)
hwTrap73: HANDLE_SYS_TRAP(0x73)
hwTrap74: HANDLE_SYS_TRAP(0x74)
hwTrap75: HANDLE_SYS_TRAP(0x75)
hwTrap76: HANDLE_SYS_TRAP(0x76)
hwTrap77: HANDLE_SYS_TRAP(0x77)
hwTrap78: HANDLE_SYS_TRAP(0x78)
hwTrap79: HANDLE_SYS_TRAP(0x79)
hwTrap7A: HANDLE_SYS_TRAP(0x7A)
hwTrap7B: HANDLE_SYS_TRAP(0x7B)
hwTrap7C: HANDLE_SYS_TRAP(0x7C)
hwTrap7D: HANDLE_SYS_TRAP(0x7D)
hwTrap7E: HANDLE_SYS_TRAP(0x7E)
hwTrap7F: HANDLE_SYS_TRAP(0x7F)

hwTrap80: HANDLE_USER_TRAP(0x80)

hwUserTrapBreakpoint:
hwTrap81:
  jmp  %l2                    // return immediately (to after the T instruction)
  rett %l2 + 4
  nop
  nop

hwUserTrapDivisionByZero:     // gcc runtime uses 'ta 2' to trigger division by zero
hwTrap82:
  b   hwTrapDivisionByZero
  nop
  nop
  nop

hwTrap83: HANDLE_USER_TRAP(0x83)
hwTrap84: HANDLE_USER_TRAP(0x84)

.global hwErrorMode
.type   hwErrorMode,#function
hwErrorMode:
.global hwUserTrapErrorMode
hwUserTrapErrorMode:
hwTrap85:
  ta HW_USER_TRAP_ERROR_MODE  // causes error mode because ET=0 when we get here
  nop
  nop
  nop

hwUserTrapSaveContext:
hwTrap86:
  /* Invoked via TA by __asmSaveContextAndCallScheduler
   * In contrast to the invocation from an interrupt, we need to return
   * to behind the TA instruction, so we reset L1 (old PC) and
   * L2 (old nPC).  The interrupt does not need that.
   */
  mov   %l2,  %l1                       // old PC  = old nPC  (return to instruction after TA)
  rd    %psr, %l0
  b     hwTrapSaveContextContinue
  add   %l2, 4, %l2                     // old nPC += 4

hwTrap87: HANDLE_USER_TRAP(0x87)
hwTrap88: HANDLE_USER_TRAP(0x88)
hwTrap89: HANDLE_USER_TRAP(0x89)
hwTrap8A: HANDLE_USER_TRAP(0x8A)


hwUserTrapEnterTrapMode: // switches from S=?, PIL=? to S=1, PIL=max.
hwTrap8B:
  rd     %psr, %i0                       // read psr
  or     %i0, HW_BITS(HW_PSR_PS) | \
              HW_BITS(HW_PSR_PIL), %l0   // ensure S=1 after RETT & PIL=max
  wr     %l0, %psr                       // set PIL=max
  nop                                    // wait for PIL to stabilise
hwTrap8C:                                // this trap is not used (if invoked, it's ignored).
  and    %i0, HW_BITS(HW_PSR_PS) | \
              HW_BITS(HW_PSR_PIL), %i0   // fix i0 for return value
  jmp    %l2                             // return to after trap instr.
  rett   %l2 + 4                         // switch ET=1 and S=1.  PIL is already set.
  nop


hwUserTrapLeaveTrapMode:  // switches from S=1, PIL=max to S=old S, PIL=old PIL
hwTrap8D:
  rd    %psr, %l0                        // get fresh value of PSR (in particular of CWP)
  andn  %l0, HW_BITS(HW_PSR_PS) | \
             HW_BITS(HW_PSR_PIL), %l0    // cut out PS and PIL
  wr    %l0, %i0, %psr                   // merge old PS and PIL, write to %psr
  nop                                    // wait for stabilisation
hwTrap8E:                                // this trap is not used (if invoked, it's ignored)
  nop
  jmp   %l2                              // return to after trap instr.
  rett  %l2 + 4                          // switch ET=1, S=old S, PIL=old PIL
  nop

hwTrap8F:
  jmp   %l2                              // jump to old nPC (=instr after TA)
  restore                                // turn back reg window wheel
  nop                                    // not RETT!
  nop

hwTrap90: HANDLE_USER_TRAP(0x90)
hwTrap91: HANDLE_USER_TRAP(0x91)
hwTrap92: HANDLE_USER_TRAP(0x92)
hwTrap93: HANDLE_USER_TRAP(0x93)
hwTrap94: HANDLE_USER_TRAP(0x94)
hwTrap95: HANDLE_USER_TRAP(0x95)
hwTrap96: HANDLE_USER_TRAP(0x96)
hwTrap97: HANDLE_USER_TRAP(0x97)
hwTrap98: HANDLE_USER_TRAP(0x98)
hwTrap99: HANDLE_USER_TRAP(0x99)
hwTrap9A: HANDLE_USER_TRAP(0x9A)
hwTrap9B: HANDLE_USER_TRAP(0x9B)
hwTrap9C: HANDLE_USER_TRAP(0x9C)
hwTrap9D: HANDLE_USER_TRAP(0x9D)
hwTrap9E: HANDLE_USER_TRAP(0x9E)
hwTrap9F: HANDLE_USER_TRAP(0x9F)
hwTrapA0: HANDLE_USER_TRAP(0xA0)
hwTrapA1: HANDLE_USER_TRAP(0xA1)
hwTrapA2: HANDLE_USER_TRAP(0xA2)
hwTrapA3: HANDLE_USER_TRAP(0xA3)
hwTrapA4: HANDLE_USER_TRAP(0xA4)
hwTrapA5: HANDLE_USER_TRAP(0xA5)
hwTrapA6: HANDLE_USER_TRAP(0xA6)
hwTrapA7: HANDLE_USER_TRAP(0xA7)
hwTrapA8: HANDLE_USER_TRAP(0xA8)
hwTrapA9: HANDLE_USER_TRAP(0xA9)
hwTrapAA: HANDLE_USER_TRAP(0xAA)
hwTrapAB: HANDLE_USER_TRAP(0xAB)
hwTrapAC: HANDLE_USER_TRAP(0xAC)
hwTrapAD: HANDLE_USER_TRAP(0xAD)
hwTrapAE: HANDLE_USER_TRAP(0xAE)
hwTrapAF: HANDLE_USER_TRAP(0xAF)
hwTrapB0: HANDLE_USER_TRAP(0xB0)
hwTrapB1: HANDLE_USER_TRAP(0xB1)
hwTrapB2: HANDLE_USER_TRAP(0xB2)
hwTrapB3: HANDLE_USER_TRAP(0xB3)
hwTrapB4: HANDLE_USER_TRAP(0xB4)
hwTrapB5: HANDLE_USER_TRAP(0xB5)
hwTrapB6: HANDLE_USER_TRAP(0xB6)
hwTrapB7: HANDLE_USER_TRAP(0xB7)
hwTrapB8: HANDLE_USER_TRAP(0xB8)
hwTrapB9: HANDLE_USER_TRAP(0xB9)
hwTrapBA: HANDLE_USER_TRAP(0xBA)
hwTrapBB: HANDLE_USER_TRAP(0xBB)
hwTrapBC: HANDLE_USER_TRAP(0xBC)
hwTrapBD: HANDLE_USER_TRAP(0xBD)
hwTrapBE: HANDLE_USER_TRAP(0xBE)
hwTrapBF: HANDLE_USER_TRAP(0xBF)
hwTrapC0: HANDLE_USER_TRAP(0xC0)
hwTrapC1: HANDLE_USER_TRAP(0xC1)
hwTrapC2: HANDLE_USER_TRAP(0xC2)
hwTrapC3: HANDLE_USER_TRAP(0xC3)
hwTrapC4: HANDLE_USER_TRAP(0xC4)
hwTrapC5: HANDLE_USER_TRAP(0xC5)
hwTrapC6: HANDLE_USER_TRAP(0xC6)
hwTrapC7: HANDLE_USER_TRAP(0xC7)
hwTrapC8: HANDLE_USER_TRAP(0xC8)
hwTrapC9: HANDLE_USER_TRAP(0xC9)
hwTrapCA: HANDLE_USER_TRAP(0xCA)
hwTrapCB: HANDLE_USER_TRAP(0xCB)
hwTrapCC: HANDLE_USER_TRAP(0xCC)
hwTrapCD: HANDLE_USER_TRAP(0xCD)
hwTrapCE: HANDLE_USER_TRAP(0xCE)
hwTrapCF: HANDLE_USER_TRAP(0xCF)
hwTrapD0: HANDLE_USER_TRAP(0xD0)
hwTrapD1: HANDLE_USER_TRAP(0xD1)
hwTrapD2: HANDLE_USER_TRAP(0xD2)
hwTrapD3: HANDLE_USER_TRAP(0xD3)
hwTrapD4: HANDLE_USER_TRAP(0xD4)
hwTrapD5: HANDLE_USER_TRAP(0xD5)
hwTrapD6: HANDLE_USER_TRAP(0xD6)
hwTrapD7: HANDLE_USER_TRAP(0xD7)
hwTrapD8: HANDLE_USER_TRAP(0xD8)
hwTrapD9: HANDLE_USER_TRAP(0xD9)
hwTrapDA: HANDLE_USER_TRAP(0xDA)
hwTrapDB: HANDLE_USER_TRAP(0xDB)
hwTrapDC: HANDLE_USER_TRAP(0xDC)
hwTrapDD: HANDLE_USER_TRAP(0xDD)
hwTrapDE: HANDLE_USER_TRAP(0xDE)
hwTrapDF: HANDLE_USER_TRAP(0xDF)
hwTrapE0: HANDLE_USER_TRAP(0xE0)
hwTrapE1: HANDLE_USER_TRAP(0xE1)
hwTrapE2: HANDLE_USER_TRAP(0xE2)
hwTrapE3: HANDLE_USER_TRAP(0xE3)
hwTrapE4: HANDLE_USER_TRAP(0xE4)
hwTrapE5: HANDLE_USER_TRAP(0xE5)
hwTrapE6: HANDLE_USER_TRAP(0xE6)
hwTrapE7: HANDLE_USER_TRAP(0xE7)
hwTrapE8: HANDLE_USER_TRAP(0xE8)
hwTrapE9: HANDLE_USER_TRAP(0xE9)
hwTrapEA: HANDLE_USER_TRAP(0xEA)
hwTrapEB: HANDLE_USER_TRAP(0xEB)
hwTrapEC: HANDLE_USER_TRAP(0xEC)
hwTrapED: HANDLE_USER_TRAP(0xED)
hwTrapEE: HANDLE_USER_TRAP(0xEE)
hwTrapEF: HANDLE_USER_TRAP(0xEF)
hwTrapF0: HANDLE_USER_TRAP(0xF0)
hwTrapF1: HANDLE_USER_TRAP(0xF1)
hwTrapF2: HANDLE_USER_TRAP(0xF2)
hwTrapF3: HANDLE_USER_TRAP(0xF3)
hwTrapF4: HANDLE_USER_TRAP(0xF4)
hwTrapF5: HANDLE_USER_TRAP(0xF5)
hwTrapF6: HANDLE_USER_TRAP(0xF6)
hwTrapF7: HANDLE_USER_TRAP(0xF7)
hwTrapF8: HANDLE_USER_TRAP(0xF8)
hwTrapF9: HANDLE_USER_TRAP(0xF9)
hwTrapFA: HANDLE_USER_TRAP(0xFA)
hwTrapFB: HANDLE_USER_TRAP(0xFB)
hwTrapFC: HANDLE_USER_TRAP(0xFC)
hwTrapFD: HANDLE_USER_TRAP(0xFD)
hwTrapFE: HANDLE_USER_TRAP(0xFE)
hwTrapFF: HANDLE_USER_TRAP(0xFF)
// end of trap table

//////////////////////////////////////////////////////////////////////
.section HW_SEC_RODATA

hwIllegalUserTrapMsg:
  .asciz "Unexpected user trap"
