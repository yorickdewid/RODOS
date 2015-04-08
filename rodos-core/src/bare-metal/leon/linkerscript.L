/*
 * (c) 2010 DLR
 *
 * Author: Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Linker Script for LEON2.
 *
 * There is another linker script for the simulator to allow faster startup.
 */
#include "hw_config.h"


/* For the simulator, we don't put the data into ROM, but directly into RAM,
 * and then have it load at start up.  We do this, because the simulator is
 * so slow that we'd be waiting for the copying from ROM to RAM for hours.
 *
 * The .bss and .stack segments are then located in RAM, too, for the
 * simulator, and will already be cleared with 0, so no memset necessary
 * either.
 */
#if HW_SIMULATOR
#  define ROM_AT(_X)
#  define ROM_ADDR(_X)  .
#  define BSS_SECTION   .data
#  define STACK_SECTION .data
#else
#  define ROM_AT(_X)    AT(_X)
#  define ROM_ADDR(_X)  (_X)
#  define BSS_SECTION   .bss
#  define STACK_SECTION .stack
#endif


/* Decide whether to also copy the code into RAM for faster
 * execution.  This requires double memory, of cause, since
 * the code is stored in ROM and there's space for it in
 * RAM, too.
 */
#if !HW_SIMULATOR && HW_CONFIG_CODE_IN_RAM
#  define STARTUP_SECTION    .boot
#  define TEXT_AT(_X)        AT(_X)
#  define TEXT_ADDR(_X)      (_X)
#  define TEXT_TARGET        RAM
#  define TRAPS_IN_RAM       HW_CONFIG_TRAPS_IN_RAM
#else
#  define STARTUP_SECTION    .text
#  define TEXT_AT(_X)
#  define TEXT_ADDR(_X)      .
#  define TEXT_TARGET        rom
#  define TRAPS_IN_RAM       0
#endif


/* If SRAM was completely disabled, move everything to SDRAM: */
#if HW_CONFIG_SRAM_ENABLE
#  define RAM sram
#else
#  define RAM sdram
#endif

/* Memory layout: */
MEMORY {
  rom   (rxai): ORIGIN = 0x00000000,            LENGTH = HW_CONFIG_PROM_SIZE
  io    (rw):   ORIGIN = 0x20000000,            LENGTH = 0x20000000
  sram  (rw):   ORIGIN = HW_CONFIG_SRAM_START,  LENGTH = HW_CONFIG_SRAM_SIZE
  sdram (rw):   ORIGIN = HW_CONFIG_SDRAM_START, LENGTH = HW_CONFIG_SDRAM_SIZE
  ahb   (rw):   ORIGIN = 0x80000000,            LENGTH = 0x80000000
}

/* Section Definitions: */
SECTIONS {
  . = HW_CONFIG_SRAM_START;

  STARTUP_SECTION :
  {
    *(.reset)

    *(.trap)
    *(.traptext)
    *(.traptext.*)
    
    *(.boot)

    . = ALIGN(8) ;
    *(.romdata)
    *(.romdata.*)

    . = ALIGN(8) ;
    __ctorsStart = . ;              /* only used at startup, so don't bother to move to RAM */
    *(SORT_BY_NAME(.ctors))
    *(SORT_BY_NAME(.init_array))
    __ctorsEnd = . ;

#if !HW_CONFIG_RODATA_IN_RAM && !HW_CONFIG_RAMIMAGE
    __RODataStart = . ;
    *(.rodata)
    *(.rodata.*)

    . = ALIGN(8);
    __RODataEnd = . ;
#endif //!HW_CONFIG_RODATA_IN_RAM && !HW_CONFIG_RAMIMAGE

#if HW_CONFIG_RAMIMAGE
  } > RAM = 0
#else
  } > rom = 0
#endif /* HW_CONFIG_RAMIMAGE */

  .text : TEXT_AT(__textROMStart)
  {
#if HW_CONFIG_RAMIMAGE
    __textROMStart = .;
#else
	__textROMStart = TEXT_ADDR( LOADADDR(.boot) + SIZEOF(.boot) ) ;
#endif //HW_CONFIG_RAMIMAGE

    __textStart = . ;
    __writeProtectStart = ABSOLUTE(.) ;

#if TRAPS_IN_RAM || HW_CONFIG_RAMIMAGE
    . = ALIGN(0x1000) + 0x10 ;
    *(.trap)
    *(.traptext)
    *(.traptext.*)
#endif	//TRAPS_IN_RAM || HW_CONFIG_RAMIMAGE

    *(.text)
    *(.text.*)

    . = ALIGN(8) ;
    *(.eh_frame*)

    . = ALIGN(8);  /* ldd/std */
    __textEnd = . ;
#if HW_CONFIG_RAMIMAGE
  } > RAM = 0
#else
  } > TEXT_TARGET = 0
#endif	//HW_CONFIG_RAMIMAGE


  .data : ROM_AT (__dataROMStart)
  {
#if HW_CONFIG_RAMIMAGE
    __dataROMStart = .;
#else
	__dataROMStart = ROM_ADDR( LOADADDR(.text) + SIZEOF(.text) ) ;
#endif	//HW_CONFIG_RAMIMAGE

    __dataStart = .;

#if HW_CONFIG_RODATA_IN_RAM || HW_CONFIG_RAMIMAGE
    __RODataStart = . ;
    *(.rodata)     /* .rodata follows .text immediately => contiguous range for write protect */
    *(.rodata.*)

    . = ALIGN(8);
    __RODataEnd = . ;
#endif

    __writeProtectEnd = ABSOLUTE(.) ;

    *(.data)
    *(.data.*)

    . = ALIGN(8);  /* ldd/std */
    __dataEnd = . ;
  } > RAM = 0


#if HW_CONFIG_RAMIMAGE
  .sdramdata :
  {
    __SDRAMDataROMStart = .;
#else
  .sdramdata : ROM_AT(__SDRAMDataROMStart)
  {
    __SDRAMDataROMStart = ROM_ADDR( LOADADDR(.data) + SIZEOF(.data) ) ;
#endif	//HW_CONFIG_RAMIMAGE

    __SDRAMDataStart = . ;

    *(.sdramdata)
    *(.sdramdata.*)

    . = ALIGN(8);
    __SDRAMDataEnd = . ;
#if HW_CONFIG_RAMIMAGE
  } > RAM = 0
#else
  } > sdram = 0
#endif	//HW_CONFIG_RAMIMAGE


  BSS_SECTION :
  {
    __bssStart = . ;
    *(.bss)
    *(.bss.*)
    . = ALIGN(8);   /* we might want to use double word zero filling */
    __bssEnd = . ;
  } > RAM AT> RAM


  STACK_SECTION :
  {
    __stackStart = . ;
    . += HW_CONFIG_STACK_SIZE ;
    . = ALIGN(8);
    __stackEnd = . ;
  } > RAM AT> RAM


  .ahb :
  {
    __AHBStart = . ;
  } > ahb
}

/* Entry Point: */
ENTRY(hwStartUp)
