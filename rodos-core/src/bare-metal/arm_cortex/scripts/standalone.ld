MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))

        *(.text .text.* .gnu.linkonce.t.*)
        *(.rodata .rodata.* .gnu.linkonce.r.*)

        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.ARM.extab* .gnu.linkonce.armextab.*)
	    *(.gcc_except_table)
	    *(.eh_frame_hdr)
	    *(.eh_frame)

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

        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))

        . = ALIGN(8);
        _etext = .;

    } >FLASH
    
	/* .ARM.exidx is sorted, so has to go in its own output section.  */
	__exidx_start = .;
	.ARM.exidx :
  	{
    	*(.ARM.exidx* .gnu.linkonce.armexidx.*)
  	} >FLASH
  	__exidx_end = .;

        .ctors :
        {
                PROVIDE(_ctors_start_ = .);
                KEEP(*(.ctors))
                KEEP(*(SORT(.ctors.*)))
                PROVIDE(_ctors_end_ = .);
          } > FLASH
	_flash_data = .;
    .data : AT (_flash_data)
    {
        _data = .;
        *(vtable)
        *(.data*)
        _edata = .;
    } > SRAM

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
        _end = .;
    	__end = .;
    } > SRAM
}
PROVIDE(_heap = _end);
PROVIDE(_eheap = ALIGN(ORIGIN(SRAM) + LENGTH(SRAM) - 8 ,8));
/* end of allocated ram _end */
PROVIDE( _HEAP_START = _end );

/* end of the heap -> align 8 byte */ 
PROVIDE ( _HEAP_END = ALIGN(ORIGIN(SRAM) + LENGTH(SRAM) - 8 ,8) );

