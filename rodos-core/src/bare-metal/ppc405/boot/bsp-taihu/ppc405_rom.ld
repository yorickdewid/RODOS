OUTPUT_FORMAT("elf32-powerpc")
OUTPUT_ARCH(powerpc:common)

SECTIONS {
  /* this makes the binary 512k in size */
  .text 0xfff80000 : {
    LONG(0);
  }
  /* we get here from hw reset */
 .rom 0xfffffffc : {
    *(rom)
  }
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
