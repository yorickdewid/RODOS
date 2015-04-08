/* The OUTPUT_FORMAT command names the BFD format to use for the output file. */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/* The entry point */
ENTRY(_start)

/* STACK SIZES */
IRQ_STACK_SIZE = 4k;
FIQ_STACK_SIZE = 4k;
SVC_STACK_SIZE = 4k;
ABT_STACK_SIZE = 4k;
UND_STACK_SIZE = 4k;
SYS_STACK_SIZE = 4k;

L1_PAGE_TABLE_SIZE = 16K * 4;
RAM_VECTORS_SIZE   = 72;
IMAGE_SPACE_SIZE   = 150k;

/* Memory alaias */
MEMORY
{
/************************************************/
/* OCRAM										*/
/************************************************/
    L1_tables (rwx) : ORIGIN = 0x00904000, LENGTH = 16K * 4
    
    TABLE0 (rwx): ORIGIN = 0x00914000, LENGTH = 100		/* Vector table of CPU0 */
    TABLE1 (rwx): ORIGIN = 0x00914064, LENGTH = 100		/* Vector table of CPU1 */
    TABLE2 (rwx): ORIGIN = 0x009140C8, LENGTH = 100		/* Vector table of CPU2 */
    TABLE3 (rwx): ORIGIN = 0x0091412C, LENGTH = 100		/* Vector table of CPU3 */

/************************************************/
/* DDR											*/
/************************************************/   
	DDR0 (rwx)  : ORIGIN = 0x10000000, LENGTH = 1M  	/* Reserved for CPU0 image */
	DDR1 (rwx)	: ORIGIN = 0x10100000, LENGTH = 1M 	/* Reserved for CPU1 image */
	DDR2 (rwx)	: ORIGIN = 0x10200000, LENGTH = 1M 	/* Reserved for CPU2 image */
	DDR3 (rwx)	: ORIGIN = 0x10300000, LENGTH = 1M 	/* Reserved for CPU3 image */
	DDR  (rwx)  : ORIGIN = 0x10400000, LENGTH = (1024M - 4M) /* This is free DDR */
}

/* Sections definition */
SECTIONS
{
/************************************************/
/* OCRAM										*/
/************************************************/
	/***************************************/
	/* MMU L1 page table
	/***************************************/
	    .l1_page_table (NOLOAD) : ALIGN(32) {
	        __l1_page_table_start = .;
	        . += L1_PAGE_TABLE_SIZE;	/* Increment the location counter with (16K * 4) */
	    } > L1_tables
	    
	/***************************************/
	/* RAM vector table for CPU3
	/***************************************/    
	    .ram_vectors (NOLOAD) : ALIGN(32) {
	        __vectors_start = .;
	        . += RAM_VECTORS_SIZE;
	        __vectors_end = .;
	    } > TABLE3



/************************************************/
/* DDR3 - CPU3									*/
/************************************************/
	/***************************************/
	/* Vector table
	/***************************************/
		.static_vector_table : ALIGN(32) {
			__static_table_start = .;
			*(.vectors)
			__static_table_end = .;
		} > DDR3
		
	/***************************************/
	/* Startup Code
	/***************************************/
		.startup : ALIGN(32) {
			__startup_start = .;
			*(.startup)
			*(.startup.secondary)
			__startup_end = .;
		} > DDR3
		
	/***************************************/
	/* Exceptions Code
	/***************************************/
		.exceptions : ALIGN(32) {
			__exceptions_start = .;
			*(.exceptions)
			__exceptions_end = .;
		} > DDR3
		
	/***************************************/
	/* Text section
	/***************************************/	
		.text : ALIGN(32) {
			__text_start = .;
		 	*(.text)
		 	*(.text.*)
			__text_end = .;
		} > DDR3
		
	/***************************************/
	/* Data section: data and read only data
	/***************************************/
		.data : ALIGN(32) {
			__data_start = .;
			*(.data)
			*(.data.*)
			*(.rodata)
			*(.rodata.*)
			__data_end = .;
		} > DDR3
		
	/***************************************/
	/* Uninitialized data section
	/***************************************/
		.bss : ALIGN(32) {
			__bss_start = .;
		    *(.bss)
		    *(.bss.*)
		    *(COMMON)
	        __bss_end = .;
	    } > DDR3
	    
	/***************************************/
	/* Runtime initialization functions
	/***************************************/
		/* Code block for initialization functions */
		.init : ALIGN(32) {
			__init_start = .;
			*(.init)
			*(.init.*)
			__init_end = .;
		} > DDR3
		
		/* Array of initialization function pointers */
		.init_array : ALIGN(32) {
			__init_array_start = .;
			LONG((__init_array_end - __init_array_start) / 4 - 2)
			*(.init_array)
			*(.init_array.*)
			LONG(0)
			__init_array_end = .;
		} > DDR3
		
	/***************************************/
	/* Runtime termination functions
	/***************************************/
		/* Code block for termination functions */
		.fini : ALIGN(32) {
			__fini_start = .;
			*(.fini)
			*(.fini.*)
			__fini_end = .;
		} > DDR3
		
		/* Array of termination function pointers */
		.fini_array : ALIGN(32) {
			__fini_array_start = .;
			LONG((__fini_array_end - __fini_array_start) / 4 - 2)	
			*(.fini_array)
			*(.fini_array.*)
			LONG(0)
			__fini_array_end = .;
		} > DDR3
		
	/***************************************/
	/* ARM sections
	/***************************************/
		.ARM.exidx : ALIGN(32) {
			__exidx_start = .;	/* I had linking errors when this was not defined */
			*(.ARM.exidx*)
			__exidx_end = .;
		} > DDR3
		
		.ARM.attributes : ALIGN(32) {
			*(.ARM.attributes)
		} > DDR3
		
	/***************************************/
	/* Constructors section
	/***************************************/
		.ctors : ALIGN(32) {
			__CTOR_LIST__ = .;
			/* This indicates the number of entries */
			LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)	
			KEEP (*crtbegin.o(.ctors))
			KEEP (*crtbegin?.o(.ctors))
			KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
			KEEP (*(SORT(.ctors.*)))
			KEEP (*(.ctors))
			LONG(0)
			__CTOR_END__ = .;
		} > DDR3
		
	/***************************************/
	/* Destructors section
	/***************************************/
		.dtors : ALIGN(32) {
			__DTOR_LIST__ = .;
			/* This indicates the number of entries */
			LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
			KEEP (*crtbegin.o(.dtors))
			KEEP (*crtbegin?.o(.dtors))
			KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
			KEEP (*(SORT(.dtors.*)))
			KEEP (*(.dtors))
			LONG(0)	
			__DTOR_END__ = .;
		} > DDR3
		
	/***************************************/
	/* Stack section
	/***************************************/
		.stacks : ALIGN(32) {
			__stack_low = . ;
	
			. += ABT_STACK_SIZE;
			. = ALIGN (8);
			__abt_stack_top__ = . ;
	
			. += UND_STACK_SIZE;
			. = ALIGN (8);
			__und_stack_top__ = . ;
			
			. += IRQ_STACK_SIZE;
			. = ALIGN (8);
			__irq_stack_top__ = . ;
	
			. += FIQ_STACK_SIZE;
			. = ALIGN (8);
			__fiq_stack_top__ = . ;
	
			. += SVC_STACK_SIZE;
			. = ALIGN (8);
			__svc_stack_top__ = . ;
	
			. += SYS_STACK_SIZE;
			. = ALIGN (8);
			__sys_stack_top__ = . ;
			
			__stack_high = .;
		} > DDR3
		
	/***************************************/
	/* Heap section
	/***************************************/
		.heap : ALIGN(32) {
			__heap_start = .;
			. += ORIGIN(DDR3) + (LENGTH(DDR3) - 4) - .;
			LONG(0xFFFFFFFF)
			__heap_end = .;
		} > DDR3
	/***************************************/
	_end = .;

/***************************************/
/* Definition of RODOS images
/***************************************/
_RODOS_image0 = ORIGIN(DDR0);
_RODOS_image1 = ORIGIN(DDR1);
_RODOS_image2 = ORIGIN(DDR2);
_RODOS_image3 = ORIGIN(DDR3);
_DDR_FREE_START = ORIGIN(DDR);
_DDR_FREE_LENGTH = LENGTH(DDR);
}