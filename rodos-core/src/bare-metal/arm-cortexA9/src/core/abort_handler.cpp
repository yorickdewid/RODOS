/*
 * Copyright (c) 2009-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @file  abort_handler.c
 *
 * @ingroup diag_init
 */
#include "workspace.h"
#include "rodos.h"


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Registers passed in from abort handler.
typedef struct {
    uint32_t ifar;
    uint32_t ifsr;
    uint32_t dfar;
    uint32_t dfsr;
    uint32_t cpsr;
    uint32_t pc;
    uint32_t sp;
    uint32_t lr;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
} arm_regs_t, *arm_regs_p;

enum {
    kDataAbortType = 0,
    kPrefetchAbortType = 1,
    kUndefinedAddress = 2,
    kError = 3
};


////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
extern "C" {

void data_exception_type(int reg);
void instruction_exception_type(int reg);

int dump_regs(int abortType, arm_regs_p regs)
{
    xprintf("\n*******************************************");
    xprintf("\n*************** System Dump ***************");
    xprintf("\n*******************************************\n");
    xprintf("Oops, %s abort occurred!\n\n", ( abortType == kDataAbortType) 		? "data" 	 :
    										( abortType == kPrefetchAbortType) 	? "prefetch" :
    										( abortType == kUndefinedAddress) 	? "undefined address" : "error");
    xprintf("Registers at point of exception:\n");
    
    // Get the name of the mode.
    const char * cpsrModeName = NULL;
    switch (regs->cpsr & CPSR_MODE)
    {
        case MODE_USR:
            cpsrModeName = "User";
            break;
        case MODE_FIQ:
            cpsrModeName = "FIQ";
            break;
        case MODE_IRQ:
            cpsrModeName = "IRQ";
            break;
        case MODE_SVC:
            cpsrModeName = "Supervisor";
            break;
        case MODE_ABT:
            cpsrModeName = "Abort";
            break;
        case MODE_UND:
            cpsrModeName = "Undefined";
            break;
        case MODE_SYS:
            cpsrModeName = "System";
            break;
    }
    
    // nzcvqeaift
    xprintf("CPSR_flags = %c%c%c%c%c%c%c%c%c%c | MODE: %s (0x%08x)\n",
        ((regs->cpsr & CPSR_N) ? 'N' : 'n'),
        ((regs->cpsr & CPSR_Z) ? 'Z' : 'z'),
        ((regs->cpsr & CPSR_C) ? 'C' : 'c'),
        ((regs->cpsr & CPSR_V) ? 'V' : 'v'),
        ((regs->cpsr & CPSR_Q) ? 'Q' : 'q'),
        ((regs->cpsr & CPSR_E) ? 'E' : 'e'),
        ((regs->cpsr & CPSR_A) ? 'A' : 'a'),
        ((regs->cpsr & CPSR_I) ? 'I' : 'i'),
        ((regs->cpsr & CPSR_F) ? 'F' : 'f'),
        ((regs->cpsr & CPSR_T) ? 'T' : 't'),
        cpsrModeName,
        (unsigned int)regs->cpsr);
        
    xprintf("r0 = 0x%08x    r8 =  0x%08x\n", (unsigned int)regs->r0, (unsigned int)regs->r8);
    xprintf("r1 = 0x%08x    r9 =  0x%08x\n", (unsigned int)regs->r1, (unsigned int)regs->r9);
    xprintf("r2 = 0x%08x    r10 = 0x%08x\n", (unsigned int)regs->r2, (unsigned int)regs->r10);
    xprintf("r3 = 0x%08x    r11 = 0x%08x\n", (unsigned int)regs->r3, (unsigned int)regs->r11);
    xprintf("r4 = 0x%08x    r12 = 0x%08x\n", (unsigned int)regs->r4, (unsigned int)regs->r12);
    xprintf("r5 = 0x%08x    sp =  0x%08x\n", (unsigned int)regs->r5, (unsigned int)regs->sp);
    xprintf("r6 = 0x%08x    lr =  0x%08x\n", (unsigned int)regs->r6, (unsigned int)regs->lr);
    xprintf("r7 = 0x%08x    pc =  0x%08x\n", (unsigned int)regs->r7, (unsigned int)regs->pc);
    
    uint32_t fsr;
    if (abortType == kDataAbortType)
    {

        xprintf("dfsr = 0x%08x\n", (unsigned int)regs->dfsr);
        xprintf("dfar = 0x%08x\n\n", (unsigned int)regs->dfar);
        fsr = regs->dfsr;
        xprintf("Access type error: %s\n", (unsigned int)(regs->dfsr & BM_DFSR_WNR) ? "write" : "read");
        xprintf("Type of exception: ");
        data_exception_type(regs->dfsr);
    }
    else if (abortType == kPrefetchAbortType)
    {
        xprintf("ifsr = 0x%08x\n", (unsigned int)regs->ifsr);
        xprintf("ifar = 0x%08x\n", (unsigned int)regs->ifar);
        fsr = regs->ifsr;
        xprintf("Type of exception: ");
        instruction_exception_type(regs->dfsr);
    }
    else
    {
    	return 0;
    }
    
    uint32_t faultStatus = ((fsr & BM_DFSR_FS4) >> BP_DFSR_FS4) | (fsr & BM_DFSR_FS);
    xprintf("Fault status: 0x%x\n", (unsigned int)faultStatus);

    return 0;
}

void data_exception_type(int reg){
	switch( (reg & BM_DFSR_FS) | ((reg & BM_DFSR_SD)>>7) | ((reg & BM_DFSR_FS4)>>6)){
	case 1:
		xprintf("Alignment fault!\n");
		break;
	case 4:
		xprintf("Instruction cache maintenance fault!\n");
		break;
	case 12:
	case 44:
		xprintf("L1 translation, precise external abort!\n");
		break;
	case 14:
	case 46:
		xprintf("L2 translation, precise external abort!\n");
		break;
	case 28:
		xprintf("L1 translation precise parity error!\n");
		break;
	case 30:
		xprintf("L2 translation precise parity error!\n");
		break;
	case 5:
		xprintf("Translation fault, section!\n");
		break;
	case 7:
		xprintf("Translation fault, page!\n");
		break;
	case 3:
		xprintf("Access flag fault, section!\n");
		break;
	case 6:
		xprintf("Access flag fault, page!\n");
		break;
	case 9:
		xprintf("Domain fault, section!\n");
		break;
	case 11:
		xprintf("Domain fault, page!\n");
		break;
	case 13:
		xprintf("Permission fault, section!\n");
		break;
	case 15:
		xprintf("Permission fault, page!\n");
		break;
	case 8:
	case 40:
		xprintf("Precise external abort, nontranslation!\n");
		break;
	case 22:
	case 54:
		xprintf("Imprecise external abort!\n");
		break;
	case 24:
		xprintf("Imprecise error, parity or ECC!\n");
		break;
	case 2:
		xprintf("Debug event!\n");
		break;
	default:
		xprintf("--\n");
		break;
	}
}

void instruction_exception_type(int reg){
	switch( (reg & BM_IFSR_FS) | ((reg & BM_IFSR_SD)>>7) | ((reg & BM_IFSR_FS4)>>6)){
	case 12:
	case 44:
		xprintf("L1 translation, precise external abort!\n");
		break;
	case 14:
	case 46:
		xprintf("L2 translation, precise external abort!\n");
		break;
	case 28:
		xprintf("L1 translation precise parity error!\n");
		break;
	case 30:
		xprintf("L2 translation precise parity error!\n");
		break;
	case 5:
		xprintf("Translation fault, section!\n");
		break;
	case 7:
		xprintf("Translation fault, page!\n");
		break;
	case 3:
		xprintf("Access flag fault, section!\n");
		break;
	case 6:
		xprintf("Access flag fault, page!\n");
		break;
	case 9:
		xprintf("Domain fault, section!\n");
		break;
	case 11:
		xprintf("Domain fault, page!\n");
		break;
	case 13:
		xprintf("Permission fault, section!\n");
		break;
	case 15:
		xprintf("Permission fault, page!\n");
		break;
	case 8:
	case 40:
		xprintf("Precise external abort, nontranslation!\n");
		break;
	case 22:
		xprintf("Imprecise external abort!\n");
		break;
	case 24:
		xprintf("Imprecise error, parity or ECC!\n");
		break;
	case 2:
		xprintf("Debug event!\n");
		break;
	default:
		xprintf("--\n");
		break;
	}
}
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
