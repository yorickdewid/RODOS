//*****************************************************************************
//
// lwipopts.h - Configuration file for lwIP
//
// Copyright (c) 2007 Luminary Micro, Inc.  All rights reserved.
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
//
// NOTE:  This file has been derived from the lwIP/src/include/lwip/opt.h
// header file.  It has been reformated to Luminary coding standards, and
// most comments have been removed.
//
// For additional details, refer to the original "opt.h" file, and lwIP
// documentation.
//
//*****************************************************************************

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__
//*****************************************************************************
//
// ---------- System options ----------
//
//*****************************************************************************
#define NO_SYS                          1       // default is 0

//*****************************************************************************
//
// ---------- Memory options ----------
//
//*****************************************************************************
#define MEM_ALIGNMENT                   4       // default is 1
#define MEM_SIZE                        (4*1024)// default is 1600
#define MEMP_NUM_NETBUF                 0       // default is 2
#define MEMP_NUM_NETCONN                0       // default is 4
#define MEMP_NUM_API_MSG                0       // default is 8
#define MEMP_NUM_TCPIP_MSG              0       // default is 8

//*****************************************************************************
//
// ---------- Pbuf options ----------
//
//*****************************************************************************
#define PBUF_POOL_SIZE                  32      // default is 16
#define PBUF_POOL_BUFSIZE               256     // default is 128
#define PBUF_LINK_HLEN                  16      // default is 14
#define ETH_PAD_SIZE                    2       // default is 0


//*****************************************************************************
//
// ---------- IP options ----------
//
//*****************************************************************************
#define IP_REASSEMBLY                   0       // default is 1
#define IP_FRAG                         0       // default is 1

//*****************************************************************************
//
// ---------- DHCP options ----------
//
//*****************************************************************************
#define LWIP_DHCP                       0       // default is 0
#define DHCP_HOST_NAME                  "fury-dev"


//*****************************************************************************
//
// ---------- TCP options ----------
//
//*****************************************************************************
//#define TCP_WND                         4096    // default is 2048
//#define TCP_MSS                         1500    // default is 128
//#define TCP_SND_BUF                     (6*TCP_MSS) // default is 256

//*****************************************************************************
//
// ---------- Application options ----------
//
//*****************************************************************************
//#define ENABLE_LMI_FS   1       // Enable the LMI File System in the
                                // sample web server application.

#endif /* __LWIPOPTS_H__ */

