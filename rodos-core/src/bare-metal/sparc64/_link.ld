OUTPUT_FORMAT("elf64-sparc")
OUTPUT_ARCH(sparc:v9a)
ENTRY(_boot)

SECTIONS {
  /* VMA AT (LMA) */
  .text 0x400000:
    AT(0x4000) {
    __start_text = .;
    *(KERNELSTART)
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t*)
    /* page aligned, PAGE_SIZE=8192 */
    . = ALIGN(0x2000);
  }
  __end_text = .;
  __data_start = .;
  .rodata : {
    /* List of con-/destructors contains:
     *    - number of entries
     *    - entries
     *    - zero
     * Note: 64-bit pointers are 8-byte in size so we use QUADs
     *    /8 function pointer is 8-bytes in size
     *    -2 minus number of entries, minus zero
     */
    __CTOR_LIST__ = .;
    QUAD((__CTOR_END__ - __CTOR_LIST__) / 8 - 2)
    *(.ctors)
    QUAD(0)
    __CTOR_END__ = .;
    /* hardcoded segment sizes; QUAD = extended word (8-bytes) */
    __size_text = .;
    QUAD(__end_text - __start_text)
    __size_data = .;
    QUAD(__data_end - __data_start);
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r*)
    . = ALIGN(0x2000);
  }
  .data : {
    *(.data)
    *(.gnu.linkonce.d*)
    . = ALIGN(0x2000);
  }
  __bss_start = .;
  .bss : {
    *(COMMON)
    *(.bss)
    *(.dynbss)
    *(.gnu.linkonce.b*)
    . = ALIGN(0x2000);
  }
  __bss_end = .;
  __data_end = .;
  _end = .;

  /DISCARD/ : {
    *(.comment)
    *(.note*)
    *(.eh_frame)
  }
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
