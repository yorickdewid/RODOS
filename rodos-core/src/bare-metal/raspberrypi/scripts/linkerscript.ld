
/* not needed by RODOS just for newlib */
_estack = 0x20020000;  
_Min_Heap_Size = 0x1000;
_Min_Stack_Size = 0x400;

MEMORY {
    rom (rx)  : ORIGIN = 0x00008000, LENGTH = 2M
    ram (rxw) : ORIGIN = 0x00208000, LENGTH = 254M
}

SECTIONS { 
    .text : { 
        * (vectors);       
        *(.text)
        *(.text*)
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */
        *(.jcr)
        *(.vfp11_veneer)
        *(.gnu.linkonce.t*)
    } > rom

   .ARM.extab : {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > rom
    
    .ARM : {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        . = ALIGN(4);
    } > rom

    .rodata : {
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r*)
        . = ALIGN(4);
    } > rom
  
    .data : {
        *(.data)
        *(.data*)
        *(.gnu.linkonce.d*)
    } > rom

    .init_array : {
        _init_array_start = .;
        KEEP (*(SORT(.init_array*)))
        _init_array_end = .;
        . = ALIGN(4);
    } > rom

    .fini_array : {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(.fini_array*))
        KEEP (*(SORT(.fini_array.*)))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);
    } > rom

    ._user_heap_stack : {
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        PROVIDE ( __heap_start__ = .);
        . = . + _Min_Heap_Size;
        PROVIDE ( __heap_end__ = .);
        . = . + _Min_Stack_Size;
        . = ALIGN(4);
    } > rom
    
    .bss : { 
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > ram

    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame)
    }
}

