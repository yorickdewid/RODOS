/* -*- Mode: C++ -*- */

/**
* @file
* @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2010 DLR
*
* @brief Implementation of trap handlers
*/

#include <rodos.h>
#include "hw_core.h"
#include "hw_trap.h"

void hwEnableInt(unsigned whichMask)
{
  unsigned old= hwEnterTrapMode();
  
  *HW_OCR(HW_OCR_INTERRUPT_MASK_AND_PRIOR)|= whichMask & 0x0000fffe;
                                             // bits 1..15 for interrupts 1..15
  hwLeaveTrapMode(old);
}

void hwDisableInt(unsigned whichMask)
{
  unsigned old= hwEnterTrapMode();
  *HW_OCR(HW_OCR_INTERRUPT_MASK_AND_PRIOR)&= ~(whichMask & 0x0000fffe);
                                             // bits 1..15 for interrupts 1..15
  hwLeaveTrapMode(old);
}

void hwClearInt(unsigned whichMask)
{
  unsigned old= hwEnterTrapMode();
  *HW_OCR(HW_OCR_INTERRUPT_CLEAR)= whichMask & 0x0000fffe;
                                   // directly writing clears the interrupts
  hwLeaveTrapMode(old);
}

