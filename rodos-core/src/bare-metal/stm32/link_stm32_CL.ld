/* Memory Spaces Definitions */

MEMORY
{
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 64K
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 256K
}


ENTRY(ResetISR)

/* Sections Definitions */

SECTIONS
{
    /* for Cortex devices, the beginning of the startup code is stored in the .isr_vector section, which goes to FLASH */
    .isr_vector :
    {
	. = ALIGN(4);
	KEEP(*(.isr_vector))            /* Startup code */
	. = ALIGN(4);
    } > FLASH

    /* the program code is stored in the .text section, which goes to Flash */
    .text :
    {
	. = ALIGN(4);
	*(.text)
	*(.text.*)
	*(.rodata)
	*(.rodata*)
	*(.glue_7)
	*(.glue_7t)
	*(.gnu.linkonce*)
	KEEP (*(.eh_frame))

    /* C++ Constructor and Destructors */
	. = ALIGN(4);
	KEEP(*(.init))

	. = ALIGN(4);
	__preinit_array_start = .;
	KEEP (*(.preinit_array))
	__preinit_array_end = .;

	. = ALIGN(4);
	__init_array_start = .;
	KEEP (*(SORT(.init_array.*)))
	KEEP (*(.init_array))
	__init_array_end = .;

	. = ALIGN(4);
	KEEP(*(.fini))

	. = ALIGN(4);
	__fini_array_start = .;
	KEEP (*(.fini_array))
	KEEP (*(SORT(.fini_array.*)))
	__fini_array_end = .;

	. = ALIGN(4);
	 _etext = .;
    } >FLASH


	__exidx_start = .;
	.ARM.exidx :
	{
	*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} >FLASH
	__exidx_end = .;

	.ARM.extab :
	{
	*(.ARM.extab)
	/* This is used by the startup in order to initialize the .data secion */
	. = ALIGN(4);
	_sidata = .;
	} >FLASH

    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH (inidata).
    It is one task of the startup to copy the initial values from FLASH to RAM. */
    .data  : AT ( _sidata )
    {
	. = ALIGN(4);
	/* This is used by the startup in order to initialize the .data secion */
	_sdata = . ;

	*(.data)
	*(.data.*)

	. = ALIGN(4);
	/* This is used by the startup in order to initialize the .data secion */
	_edata = . ;
    } >RAM

    /* This is the uninitialized data section */
    .bss :
    {
	. = ALIGN(4);
	/* This is used by the startup in order to initialize the .bss secion */
	_sbss = .;
	__bss_start__ =.;
	
	*(.bss)
	*(COMMON)

	. = ALIGN(4);
	/* This is used by the startup in order to initialize the .bss secion */
	_ebss = .;
	__bss_end__ = .;
    } >RAM

    PROVIDE ( end = _ebss );
    PROVIDE ( _end = _ebss );
	
	PROVIDE(_estack = ALIGN(ORIGIN(RAM) + LENGTH(RAM) - 8 ,8));

/*	PROVIDE(_heap = _end);*/
/*	PROVIDE(_eheap = ALIGN(ORIGIN(RAM) + LENGTH(RAM) - 8 ,8));*/

	/DISCARD/ :
	{
	}
}
