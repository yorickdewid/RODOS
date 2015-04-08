/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief OpenBoot PROM routine to acquire information about the cpu.
 */

#include <asi.h>
#include <printk.h>
#include <oplib.h>
#include <string_pico.h>

prom_cpuinfo prom_cpus[MAX_CPUS];

uint32_t prom_finddevice_by_type(uint32_t node, char *name, int n) {
  uint32_t c;
  char p[64];

  for (c = prom_getchild(node); c; c = prom_getpeer(c)) {
    prom_getprop(c, "device_type", p, sizeof(p));
    if (p[0]==0)
      continue;
    if (strncmp(p, name, n) == 0)
      return c;
  }

  return 0;
}

void prom_init_cpu() {
  uint64_t current_mid;
  uint32_t prom_cpunode;
  uint32_t clock, cnode;
  char ntype[128];
  int mid, cpu_count = 0;

  /**
   * The UPA Configuration register contains the MID[21:17] field.
   * MID as in Module (processor) ID.
   */
  asm volatile("ldxa [%1] %2, %0\n"
          : "=r" (current_mid)
          : "r"  (0), "i" (ASI_UPA_CONFIG));

  current_mid = (current_mid >> 17) & 0b11111; /* MID[21:17] */

  prom_cpunode = prom_finddevice_by_type(node_root, "cpu", 3);
  if (!prom_cpunode) {
    printk("Note: cpu device not found.\n");
    prom_cpunode = node_root;
  }

  /**
   * NOTE for future SMP support.
   * For SMP we need prom_cpunode to be the parent node of the cpu devices.
   */

  prom_getprop(prom_cpunode, "device_type", ntype, sizeof(ntype));

  if(strncmp(ntype, "cpu", 3) == 0) {
    /* On a singlecore system we should get here. */
    prom_cpus[0].node = prom_cpunode;
    prom_cpus[0].mid = 0;
    prom_getprop(prom_cpus[0].node,
                 "clock-frequency",
                 &clock, sizeof(clock));
      prom_cpus[0].clock = clock;
    cpu_count++;
  } else {
    /* On a multicore system we should get here (again, untested). */
    cnode = prom_getchild(prom_cpunode);
    while((cnode = prom_getchild(cnode)) != 0) {
      prom_getprop(cnode, "device_type", ntype, sizeof(ntype));
      if(strncmp(ntype, "cpu", 3) == 0) {
        prom_cpus[cpu_count].node = cnode;
        /**
        * Note, this property changes with different CPU families:
        * US I and II    - upa-portid
        * US III         - portid
        * US IV          - cpuid
        */
        prom_getprop(cnode, "upa-portid",
                     (char *) &mid,
                     sizeof(mid));
        prom_cpus[cpu_count].mid = mid;
        prom_getprop(cnode,
                    "clock-frequency",
                    &clock, sizeof(clock));
        prom_cpus[cpu_count].clock = clock;
        cpu_count++;
      }
    }
  }

  if(cpu_count == 0) {
    printk("Error: No CPU nodes found.\n");
    prom_exit();
  }
}
