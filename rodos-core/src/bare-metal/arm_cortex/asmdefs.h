//*****************************************************************************
//
// asmdefs.h - Macros to allow assembly code be portable among toolchains.
//
// Copyright (c) 2005-2007 Luminary Micro, Inc.  All rights reserved.
// 
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 1900 of the Stellaris Peripheral Driver Library.
//
//*****************************************************************************

#ifndef __ASMDEFS_H__
#define __ASMDEFS_H__


//*****************************************************************************
//
// The defines required for GCC.
//
//*****************************************************************************

//
// The assembly code preamble required to put the assembler into the correct
// configuration.
//
    .syntax unified
    .thumb

//
// Section headers.
//
#define __LIBRARY__             @
#define __TEXT__                .text
#define __DATA__                .data
#define __BSS__                 .bss
#define __TEXT_NOROOT__         .text

//
// Assembler nmenonics.
//
#define __ALIGN__               .balign 4
#define __END__                 .end
#define __EXPORT__              .globl
#define __IMPORT__              .extern
#define __LABEL__               :
#define __STR__                 .ascii
#define __THUMB_LABEL__         .thumb_func
#define __WORD__                .word


#endif // __ASMDEF_H__
