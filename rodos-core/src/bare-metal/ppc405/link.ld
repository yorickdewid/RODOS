OUTPUT_FORMAT("elf32-powerpc")
OUTPUT_ARCH(powerpc:common)
ENTRY(_boot)

SECTIONS {

  /* EVPR starts at 0x0 */
  . = 0x0000;

  .boot : {
    *(KERNELSTART)
  }

  /* offset */
  . = 0x4000;

  .text : {
    *(.startup)
    *(.text)
    *(.text.*)
    *(.got1)
    *(.got2)
    . = ALIGN(4);
  }

  .rodata : {
    /* List of con-/destructors needs to be page aligned and contains:
     *    - number of entries
     *    - entries
     *    - zero
     */
    __CTOR_LIST__ = .;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
    *(.ctors)
    LONG(0)
    __CTOR_END__ = .;
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r*)
    . = ALIGN(4);
  }

  .data : {
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d*)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(4);
  }

  .bss : {
    __bss_start = .;
    *(.scommon)
    *(.sbss)
    *(.sbss.*)
    *(COMMON)
    *(.bss)
    *(.bss.*)
    *(.dynbss)
    *(.gnu.linkonce.b*)
    __bss_end = .;
    . = ALIGN(4);
  }

  /* 1M boot stack */
  . = ALIGN(0xff);
  . += 0x100000;
  _stack = .;

  /DISCARD/ : {
    *(.comment)
    *(.note*)
    /* we do not have runtime support for C++ exceptions */
    *(.eh_frame)
  }
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
