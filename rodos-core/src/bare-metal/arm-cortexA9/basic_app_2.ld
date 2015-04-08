/* The OUTPUT_FORMAT command names the BFD format to use for the output file. */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/* The entry point */
ENTRY(_start)

/* STACK SIZES */
IRQ_STACK_SIZE = 1K;
FIQ_STACK_SIZE = 1K;
SVC_STACK_SIZE = 1K;
ABT_STACK_SIZE = 1K;
UND_STACK_SIZE = 1K;
SYS_STACK_SIZE = 1K;

L1_PAGE_TABLE_SIZE = 16K;
RAM_VECTORS_SIZE   = 72;
IMAGE_SPACE_SIZE   = 200M;

/* Memory alaias */
MEMORY
{
    OCRAM (rwx) : ORIGIN = 0x00900000, LENGTH = L1_PAGE_TABLE_SIZE
    
    OCRAM0 (rwx): ORIGIN = 0x00904000, LENGTH = RAM_VECTORS_SIZE
    OCRAM1 (rwx): ORIGIN = 0x00904114, LENGTH = RAM_VECTORS_SIZE
    OCRAM2 (rwx): ORIGIN = 0x00904228, LENGTH = RAM_VECTORS_SIZE
    OCRAM3 (rwx): ORIGIN = 0x0090433C, LENGTH = RAM_VECTORS_SIZE
    
	DDR0 (rwx)  : ORIGIN = 0x10000000, LENGTH = IMAGE_SPACE_SIZE
	DDR1 (rwx)	: ORIGIN = 0x1C800000, LENGTH = IMAGE_SPACE_SIZE
	DDR2 (rwx)	: ORIGIN = 0x29000000, LENGTH = IMAGE_SPACE_SIZE
	DDR3 (rwx)	: ORIGIN = 0x35800000, LENGTH = IMAGE_SPACE_SIZE
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
	        . += L1_PAGE_TABLE_SIZE;	/* Increment the location counter with 16K */
	    } > OCRAM
	    
	/***************************************/
	/* RAM vector table for each CPU
	/***************************************/    
	    .ram_vectors_cpu0 (NOLOAD) : ALIGN(32) {
	        __vectors_cpu0_start = .;
	        . += RAM_VECTORS_SIZE;
	        __vectors_cpu0_end = .;
	    } > OCRAM
	    .ram_vectors_cpu1 (NOLOAD) : ALIGN(32) {
	        __vectors_cpu1_start = .;
	        . += RAM_VECTORS_SIZE;
	        __vectors_cpu1_end = .;
	    } > OCRAM
	    .ram_vectors_cpu2 (NOLOAD) : ALIGN(32) {
	        __vectors_cpu2_start = .;
	        . += RAM_VECTORS_SIZE;
	        __vectors_cpu2_end = .;
	    } > OCRAM
	    .ram_vectors_cpu3 (NOLOAD) : ALIGN(32) {
	        __vectors_cpu3_start = .;
	        . += RAM_VECTORS_SIZE;
	        __vectors_cpu3_end = .;
	    } > OCRAM



/************************************************/
/* DDR - CPU0									*/
/************************************************/
	/***************************************/
	/* Vector table
	/***************************************/
		.static_vector_table : ALIGN(32) {
			*(.vectors)
		} > DDR
		
	/***************************************/
	/* Startup Code
	/***************************************/
		.startup_cpu0 : ALIGN(32) {
			__startup_cpu0_start = .;
			*(.startup)
		} > DDR
		
	/***************************************/
	/* Exceptions Code
	/***************************************/
		.exceptions_cpu0 : ALIGN(32) {
			__exceptions_cpu0_start = .;
			*(.exceptions)
		} > DDR
		
	/***************************************/
	/* Text section
	/***************************************/	
		.text_cpu0 : ALIGN(32) {
			__text_cpu0_start = .;
		 	*(.text)
		 	*(.text.*)
			__text_cpu0_end = .;
		} > DDR
		
	/***************************************/
	/* Data section: data and read only data
	/***************************************/
		.data_cpu0 : ALIGN(32) {
			__data_cpu0_start = .;
			*(.data)
			*(.data.*)
			*(.rodata)
			*(.rodata.*)
			__data_cpu0_end = .;
		} > DDR
		
	/***************************************/
	/* Uninitialized data section
	/***************************************/
		.bss_cpu0 : ALIGN(32) {
			__bss_cpu0_start = .;
		    *(.bss)
		    *(.bss.*)
		    *(COMMON)
	        __bss_cpu0_end = .;
	    } > DDR
	    
	/***************************************/
	/* Runtime initialization functions
	/***************************************/
		/* Code block for initialization functions */
		.init_cpu0 : ALIGN(32) {
			__init_cpu0_start = .;
			*(.init)
			*(.init.*)
			__init_cpu0_end = .;
		} > DDR
		
		/* Array of initialization function pointers */
		.init_array_cpu0 : ALIGN(32) {
			__init_array_cpu0_start = .;
			LONG((__init_array_cpu0_end - __init_array_cpu0_start) / 4 - 2)
			*(.init_array)
			*(.init_array.*)
			LONG(0)
			__init_array_cpu0_end = .;
		} > DDR
		
	/***************************************/
	/* Runtime termination functions
	/***************************************/
		/* Code block for termination functions */
		.fini_cpu0 : ALIGN(32) {
			__fini_cpu0_start = .;
			*(.fini)
			*(.fini.*)
			__fini_cpu0_end = .;
		} > DDR
		
		/* Array of termination function pointers */
		.fini_array_cpu0 : ALIGN(32) {
			__fini_array_cpu0_start = .;
			LONG((__fini_array_cpu0_end - __fini_array_cpu0_start) / 4 - 2)	
			*(.fini_array)
			*(.fini_array.*)
			LONG(0)
			__fini_array_cpu0_end = .;
		} > DDR
		
	/***************************************/
	/* ARM sections
	/***************************************/
		.ARM.exidx_cpu0 : ALIGN(32) {
			__exidx_start_cpu0 = .;	/* I had linking errors when this was not defined */
			*(.ARM.exidx*)
			__exidx_end_cpu0 = .;
		} > DDR
		
		.ARM.attributes_cpu0 : ALIGN(32) {
			*(.ARM.attributes)
		} > DDR
		
	/***************************************/
	/* Constructors section
	/***************************************/
		.ctors_cpu0 : ALIGN(32) {
			__CTOR_LIST_cpu0__ = .;
			/* This indicates the number of entries */
			LONG((__CTOR_END_cpu0__ - __CTOR_LIST_cpu0__) / 4 - 2)	
			KEEP (*crtbegin.o(.ctors))
			KEEP (*crtbegin?.o(.ctors))
			KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
			KEEP (*(SORT(.ctors.*)))
			KEEP (*(.ctors))
			LONG(0)
			__CTOR_END_cpu0__ = .;
		} > DDR
		
	/***************************************/
	/* Destructors section
	/***************************************/
		.dtors_cpu0 : ALIGN(32) {
			__DTOR_LIST_cpu0__ = .;
			/* This indicates the number of entries */
			LONG((__DTOR_END_cpu0__ - __DTOR_LIST_cpu0__) / 4 - 2)
			KEEP (*crtbegin.o(.dtors))
			KEEP (*crtbegin?.o(.dtors))
			KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
			KEEP (*(SORT(.dtors.*)))
			KEEP (*(.dtors))
			LONG(0)	
			__DTOR_END_cpu0__ = .;
		} > DDR
		
	/***************************************/
	/* Stack section
	/***************************************/
		.stacks_cpu0 : ALIGN(32) {
			__stack_cpu0_low = . ;
	
			. += ABT_STACK_SIZE;
			. = ALIGN (8);
			__abt_stack_cpu0_top__ = . ;
	
			. += UND_STACK_SIZE;
			. = ALIGN (8);
			__und_stack_cpu0_top__ = . ;
			
			. += IRQ_STACK_SIZE;
			. = ALIGN (8);
			__irq_stack_cpu0_top__ = . ;
	
			. += FIQ_STACK_SIZE;
			. = ALIGN (8);
			__fiq_stack_cpu0_top__ = . ;
	
			. += SVC_STACK_SIZE;
			. = ALIGN (8);
			__svc_stack_cpu0_top__ = . ;
	
			. += SYS_STACK_SIZE;
			. = ALIGN (8);
			__sys_stack_cpu0_top__ = . ;
			
			__stack_cpu0_high = .;
		} > DDR
		
	/***************************************/
	/* Heap section
	/***************************************/
		.heap_cpu0 : ALIGN(32) {
			__heap_cpu0_start = .;
			. = . + 10M;
			__heap_cpu0_end = .;
		} > DDR
		
	/***************************************/
}