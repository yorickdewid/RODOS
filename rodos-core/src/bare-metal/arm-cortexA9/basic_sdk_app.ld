/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
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

/* The OUTPUT command names the output file. 
OUTPUT (cortexA9.out) */

/* The OUTPUT_FORMAT command names the BFD format to use for the output file. */
OUTPUT_FORMAT("elf32-littlearm")

/* Specify a particular output machine architecture.
OUTPUT_ARCH(arm) */

/* The first instruction to execute in a program is called the entry point.
The ENTRY linker script command is used to set the entry point. 
In this case the '_start'*/
ENTRY(_start)

/* The INPUT command directs the linker to include the named files in the link. */
INPUT (/opt/arm-2012.03/lib/gcc/arm-none-eabi/4.6.3/crtbegin.o )
INPUT (/opt/arm-2012.03/lib/gcc/arm-none-eabi/4.6.3/crti.o )
INPUT (/opt/arm-2012.03/lib/gcc/arm-none-eabi/4.6.3/crtend.o )
INPUT (/opt/arm-2012.03/lib/gcc/arm-none-eabi/4.6.3/crtn.o )


/* The GROUP command is like INPUT, except that the named files should all be archives.
Maybe I will need to include the librodos.a: 
GROUP (/home/admin/MyWorspace/rodos-101/libs/arm-cortexA9/librodos.a) */


/* OCRAM */
OCRAM_ORG               = 0x00900000;
OCRAM_LEN               = 256K; 	/* = 262144 	= 256K 	*/

/* DDR */
DDR_ORG                 = 0x10000000;
DDR_LEN                 = 2048M; 	/* = 1073741824 = 1024M */
RAM_VECTORS_SIZE        = 72;
L1_PAGE_TABLE_SIZE      = 0x00004000;	/* = 16384 	= 16K 	*/
MAX_IMAGE_SIZE          = 0x00400000; 	/* = 4194304 	= 4M 	*/
MAX_HAB_CSF_DATA_SIZE   = 0x00002000;	/* = 8192 	= 8K 	*/
STACK_SIZE              = 0x0000C000;	/* = 49152	= 48K 	*/



/* Describes the location and size of blocks of memory in the board. */
MEMORY
{
    OCRAM (rwx) : ORIGIN = 0x00900000, 	LENGTH = 256K
    DDR (rwx)   : ORIGIN = 0x10000000,  LENGTH = 1024M
}


/* Tells the linker how to map input sections into output sections,
and how to place the output sections in memory. */
SECTIONS
{
/*
 * -- OCRAM --
 *
 * Nothing in OCRAM can be loaded at boot, because the boot image must be a contiguous
 * region of memory.

LAYOUT:
		 _______________________
.l1_page_table	|__l1_page_table_start	| OCRAM_ORG 				= 0x00900000
		|			|
		|_______________________|
.heap.ocram	|__heap_ocram_start	| OCRAM_ORG + 16K 			= 0x00904000
		|__heap_ocram_end	|
		|_______________________|
.ram_vectors	|__ram_vectors_start	| (OCRAM_ORG + 16K) + 256K - 16K - 72   = 0x0093FFB8
		|__ram_vectors_end	|
		|_______________________|
				 	  OCRAM_END				= 0x00940000
*/

    /* MMU L1 page table */
    /* The section l1_page_table will be set to the current value of the location counter + the OCRAM start */
    .l1_page_table (NOLOAD) :
    {
        __l1_page_table_start = .;	/* Set __l1_page_table_start to the beginning of OCRAM */
        . += L1_PAGE_TABLE_SIZE;	/* Increment the location counter with 16K */
    } > OCRAM

    /* allocate a heap in ocram */
    .heap.ocram (NOLOAD) : ALIGN(4)
    {
        __heap_ocram_start = .;
        . += LENGTH(OCRAM) - L1_PAGE_TABLE_SIZE	 - RAM_VECTORS_SIZE;
        __heap_ocram_end = .;
    } > OCRAM
    
    /* RAM vector table comes at the end of OCRAM */
    .ram_vectors (ORIGIN(OCRAM) + LENGTH(OCRAM) - RAM_VECTORS_SIZE) (NOLOAD) :
    {
        __ram_vectors_start = .;
        . += RAM_VECTORS_SIZE;
        __ram_vectors_end = .;
    } > OCRAM


/*
 * -- DDR --
 *
LAYOUT:
		 _______________________
.ivt		|_start_image_add	| DDR_ORG 				= 0x10000000
		|-----------------------|
		|*(.ivt)                | DDR_ORG + 0x400			= 0x10000400 
		|_______________________|
.boot_data	|*(.boot_data)          |__start_boot_data
		|_______________________|
.dcd_hdr	|*(.dcd_hdr)            | __start_dcd
		|_______________________|
.dcd_wrt_cmd	|*(.dcd_wrt_cmd)        |
		|_______________________|
.dcd_data	|*(.dcd_data)           |
		|_______________________|
.init		|KEEP (*(.init))        | > DDR = 0
		|_______________________|
.text		|*(.startup)		| - This code is defined int startup.S: _start function
		|*(.text .text.*)	|
		|_______________________|
.fini		|KEEP (*(.fini))        | > DDR = 0
		|_______________________|
.ctors		|KEEP (*crtbegin.o(.ctors))
		|KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
		|KEEP (*(SORT(.ctors.*)))
		|KEEP (*(.ctors))       
		|_______________________
.dtors		|KEEP (*crtbegin.o(.dtors))
		|KEEP (*crtbegin?.o(.dtors))
		|KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
		|KEEP (*(SORT(.dtors.*)))
		|KEEP (*(.dtors))
		|_______________________|
.rodata		|*(.rodata .rodata.*)	|
		|_______________________|
.ARM.exidx	|*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		|_______________________|
.data		|*(.data .data.*)	|
        	|SORT(CONSTRUCTORS)	|
		|_______________________|
		|-----------------------|
		|_______________________|
.hab.data	|			| __hab_data 		= _start_image_add + 4M/*MAX_IMAGE_SIZE
        	|			|
		|_______________________| __hab_data_end	= __hab_data + 8K

At this momment is defined the image siye: _image_size = . - _start_image_add;

		 _______________________
.bss		|*(.shbss)		| __bss_start__
		|*(.bss .bss.* .gnu.linkonce.b.*)
		|*(COMMON)		|
		|*(.bss)		|
		|_______________________| __bss_end__
.stacks		|			|
		|			|
		|_______________________| top_of_stacks		= __bss_end__ + 48K
.heap		|			| free_memory_start
		|			|
		|_______________________| free_memory_end	= ORIGIN(DDR) + LENGTH(DDR) - .
					  _end
*/

    /* -- read-only sections -- */
    
    _start_image_add = ORIGIN(DDR);
    
    .ivt (ORIGIN(DDR)) :
    {
        . = . + 0x400;
        *(.ivt)
    } > DDR
    
    .boot_data :
    {
        __start_boot_data = .;
        *(.boot_data)
    } > DDR
    
    /* aligned to ease the hexdump read of generated binary */ 
    .dcd_hdr : ALIGN(16)
    {
        __start_dcd = .;
        *(.dcd_hdr)
    } > DDR
    .dcd_wrt_cmd :
    {
        *(.dcd_wrt_cmd)
    } > DDR
    .dcd_data :
    {
        *(.dcd_data)
    } > DDR

    .init :
    {
        KEEP (*(.init))
    } > DDR =0
    
    .text : ALIGN(8)
    {
        *(.startup)
        *(.text .text.*)
    } > DDR

    .fini :
    {
        KEEP (*(.fini))
    } > DDR =0

    /* Section for CONTRUCTORS - for more info:
       doc.ironwoodlabs.com/arm-arm-none-eabi/html/ld.html/Output-Section-Keywords.html#Output-Section-Keywords*/
    .ctors :
    {
        /* gcc uses crtbegin.o to find the start of
           the constructors, so we make sure it is
           first.  Because this is a wildcard, it
           doesn't matter if the user does not
           actually link against crtbegin.o; the
           linker won't look for a file to match a
           wildcard.  The wildcard also means that it
           doesn't matter which directory crtbegin.o
           is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        /* We don't want to include the .ctor section from
           the crtend.o file until after the sorted ctors.
           The .ctor section from the crtend file contains the
           end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    } > DDR
    /* Section for DESTRUCTORS */
    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    } > DDR

    .rodata :
    {
        *(.rodata .rodata.*)
    } > DDR

    /* Unwind index. This section is related to C++ exceptions, and is required even
       though exceptions are disabled with -fno-exceptions. */
    PROVIDE_HIDDEN(__exidx_start = .);
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > DDR
    PROVIDE_HIDDEN(__exidx_end = .);

    /* -- read-write sections -- */
    
    .data :
    {
        *(.data .data.*)
        SORT(CONSTRUCTORS)
    } > DDR

    /* Reserve some space for HAB CSF data */
    .hab.data (_start_image_add + 4M/*MAX_IMAGE_SIZE*/) : ALIGN(4)
    {
        __hab_data = .;
        . += 8K/*MAX_HAB_CSF_DATA_SIZE*/;
        . = ALIGN (32);
        __hab_data_end = .;
    } > DDR
    /* End of HAB reserved space (must place it before BSS section) */

    _image_size = . - _start_image_add;

    /* The .bss section comes after the hab data because it is not signed */
    .bss :
    {
        __bss_start__ = .;
        *(.shbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)	/* To place common symbols */
        *(.bss)
        __bss_end__ = .;
    } > DDR

    /* Region to contain exception handler stacks */
    .stacks (NOLOAD) :
    {
        . += 48K/*STACK_SIZE*/;
        top_of_stacks = .;
    } > DDR
    
    /* define section for the malloc heap */
    .heap (NOLOAD) : ALIGN(32)
    {
        free_memory_start = .; /* malloc starts allocating from here */
        . += ORIGIN(DDR) + LENGTH(DDR) - .;
        free_memory_end = .;
    } > DDR

    _end = .;
}
