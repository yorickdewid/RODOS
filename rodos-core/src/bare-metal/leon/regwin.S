/* -*- Mode: Assembler */

/**
 * @file
 * @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Copyright 2008-2010 DLR
 *
 * @brief Register window stuff
 */

#include "hw_asmdef.h"

.section HW_SEC_TRAP_TEXT

.global hwTrapWindowOverflowContinue
.type   hwTrapWindowOverflowContinue, #function
hwTrapWindowOverflowContinue:
  /* Trap handler looks like this (crt.S):

  rd    %wim, %l3
  mov   %g1, %l7
  b     hwTrapWindowOverflowContinue
  srl   %l3, 1, %g1

  */

  sll   %l3, HW_CONFIG_REG_WINDOWS - 1, %l3     // complete rotatation right by 1 bit

  save %l3, %g1, %g1                            // do the window saving and move new WIM to %g1
    wr  %g1, %wim                               // set new WIM: max. 3 instrs until it is stable
    NOP_WAIT0                                   // for WIM
    SAVE_WINDOW(%sp)
  restore

  mov   %l7, %g1                                // restore G1

  jmp   %l1
  rett  %l2


.global hwTrapWindowUnderflowContinue
.type   hwTrapWindowUnderflowContinue, #function
hwTrapWindowUnderflowContinue:
  /* Trap handler loops like this (crt.S):

  rd    %wim, %l3
  sll   %l3, 1, %l7
  b     hwTrapWindowUnderflowContinue
  srl   %l3, HW_CONFIG_REG_WINDOWS - 1, %l3

  */

  wr %l3, %l7, %wim                             // %l3 xor %l7 does the job: set new WIM
  NOP_WAIT2                                     // for WIM

  restore                                       // WIM (or old WIM)!
    restore                                     // WIM!
      LOAD_WINDOW(%sp)
    save
  save

  jmp   %l1
  rett  %l2


