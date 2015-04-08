
#pragma once

/** 
This file  is just a place holder. 
It has no function for rodos.
it just help to show you some variables witch you may redefined when
compiling rodos.

DO NOT MODIFIY THIS FILE: create your own version, see readme.txt

**/

// use following variables to see what you have compiled!
#define USER_CONFIG_TEXT     "userconfig: default userconfig.h no settings at all: Version 0"
#define USER_CONFIG_VERSION  0

// you may set / change followin parameter (but NOT HERE! Do not modify this file, see readme.txt)
// To Modfy any thing, please first check the default values in the correspoding parmans.h 



// #define I2C_TIMEOUT_IN_NS               (3*MILLISECONDS)


//#define CPU_CLK                         168   // 24,32,42,48,56,84,96,120,144,168 MHz
//#define UART_DEBUG                      UART_IDX2, GPIO_053, GPIO_054 // PD5 and PD6
//#define XMALLOC_SIZE                    40*1024
//#ifndef DEFAULT_STACKSIZE               2000
//#define SCHEDULER_STACKSIZE             0
//#define PARAM_TIMER_INTERVAL            10000
//#define TIME_SLICE_FOR_SAME_PRIORITY    (100*MILLISECONDS)
//#define DEFAULT_THREAD_PRIORITY         100
//#define MAX_THREAD_PRIORITY             1000
//#define NETWORKREADER_PRIORITY          (MAX_THREAD_PRIORITY + 2)
//#define CEILING_PRIORITY                (NETWORKREADER_PRIORITY + 1)
//#define MAX_NUMBER_OF_NODES             10
//#define MAX_NETWORK_MESSAGE_LENGTH      1300
//#define MAX_SUBSCRIBERS                 60





//eg 192.168.1.255. All computer in  network 192.168.1.xxx receive it. (your home network)
//   127.255.255.255 broadcast of loopback: only to local host
//   255.255.255.255 The whole local network

//#define IP_BROADCAST_ADR	"192.168.1.255"
//#define IP_BROADCAST_ADR	"255.255.255.255"
#define IP_BROADCAST_ADR	"127.255.255.255"


