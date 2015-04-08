/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Bootstrapping and compatibility checks.
 */

#include <pstate.h>
#include <param.h>
#include <oplib.h>
#include <printk.h>
#include <tlb.h>
#include <tsb.h>

#include "params.h"

extern uint32_t sparc_ramdisk_image;
extern uint32_t sparc_ramdisk_size;
extern uint64_t sparc_ramdisk_image64;

extern void *__data_end;

void bootstrap(uint64_t ver) {
  int ch=0, i=0;// pt_size;
  char buffer[8];
  /* TODO Memory management */
  //pte_t page_table_bootstrap[1024]; /* Phase 1 page table for 8M memory */

  /** Extract PROM version, something like 'OBP xx.yy.zz ...' */
  while((ch = prom_version[4+i]) != ' ' && i < 8)
    buffer[i++] = ch;
  buffer[i] = '\0';

  printk("Sun IEEE P1275 PROM Version %s\n", buffer);
  printk("SPARC64 subarchitecture '%s'\n", prom_subarch);
  printk("Bootpath: %s Bootargs: %s\n", prom_bootpath, prom_bootargs);

  /**
   * Check compatibility with CPU Architecture:
   * Extract version info from %ver register for comparison with
   * HW_ARCH_<CPU_CODENAME> identifier. If at some point more SPARCv9
   * architectures are supported, just add a case with it's identifier here.
   */
  switch((ver&VERS_IMPL) >> 32) {
    case HW_ARCH_BLACKBIRD:
      /* This will also be the case for SIMICS sunfire target. */
      printk("CPU: UltraSPARC-II (Blackbird)\n");
      break;

    case HW_ARCH_SABRE:
      /* This will also be the case for QEMU emulator sun4u. */
      printk("CPU: UltraSPARC-IIi (Sabre)\n");
      break;

    case HW_ARCH_HUMMINGBIRD:
      printk("CPU: UltraSPARC-IIe (Hummingbird)\n");
      break;

    default:
      printk("Error: CPU (id 0x%x) is not yet supported by RODOS.\n",ver);
      prom_exit();
  }

  /**
   * Try to read manufacturer code, this field is optional and may be zero.
   */
  switch((ver&VERS_MANUF) >> 48) {
    case HW_VENDOR_ULTRASPARC:
      printk("Manufacturer code: UltraSPARC\n");
      break;

    case HW_VENDOR_SUN:
      printk("Manufacturer code: Sun\n");
      break;

    case HW_VENDOR_FUJITSU:
      printk("Manufacturer code: Fujitsu\n");
      break;

    case 0:
      printk("Manufacturer code not set\n");
      break;

    default:
      printk("Manufacturer code: unknown (0x%x)\n",(ver&VERS_MANUF) >> 48);
  }

  /**
   * CPU is supported if we get here, now its time to initialize it.
   */
  prom_init_cpu();

  /* Print CPUid and Clock-frequency for every detected cpu core. */
  for(i = 0; i< MAX_CPUS; i++) {
    if(prom_cpus[i].node != 0) {
      printk("Core id %d operating at %dMHz\n",
             prom_cpus[i].mid,
             prom_cpus[i].clock/1000000);
    }
  }

  /** Print architecture specific register properties. */
  /* max contents of the trap level register */
  printk("Arch: max TL=%d, ",(ver&VERS_MAXTL) >> 8);
  /* max valid CWP = NWINDOWS-1 */
  printk("max valid CWP=%d\n",ver&VERS_MAXWIN);

  if((ver&VERS_MAXWIN) != NWINDOWS-1) {
    printk("Error: Register window configuration does not match.\n");
    prom_exit();
  }

  /**
   * The initialization of the page table is divided up into two phases. At
   * first, the bootstrap phase is setting up a page table for XXX M so the
   * paging unit can be enabled. The second phase then calls paging_init() to
   * create the inverted page table depending on the size of available memory.
   */
  /* TODO */

  /** initialize the TSB */
  //init_tsb();

  /* invalidate unlocked tlb entries */
  invalidate_itlb_all();
  invalidate_dtlb_all();

  /*
  pt_size = i = memlist_total.total >> PAGE_SHIFT;
  ch=0;
  while(i >>=1)
    ch++;
  printk("Page table with %d entries (%dbit) takes %dK\n",
         pt_size,
         ch,
         (sizeof(pte_t)*pt_size) >> 10);
  */
  //debug_print_dtlb();
  //debug_print_itlb();
  //debug_print_sfsr();
  printk("\n");
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
