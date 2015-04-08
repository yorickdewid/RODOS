/**
 * @file
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Absolute jump to kernel, necessary for QEMU ppc405 "bios" ROM.
 */

  .section    .rom, "aw"
  ba      0x0

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
