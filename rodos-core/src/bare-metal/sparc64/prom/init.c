/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief The OpenBoot PROM client interface.
 * This file contains routines concerning the Sun IEEE 1275 PROM low level
 * interface.
 */

#include <printk.h>
#include <oplib.h>
#include <string_pico.h>

uint32_t node_root;
uint32_t node_chosen;
uint32_t node_stdout;
uint32_t node_stdin;
uint32_t node_obp;

char prom_version[PROM_MAXVERSLEN];
char prom_bootpath[PROM_MAXBARGLEN];
char prom_bootargs[PROM_MAXBARGLEN];
char prom_subarch[16];

/**
 * @brief Initializes the OpenBoot PROM interface on an early boot stage.
 * Before using any routines in the prom library, prom_init() must be
 * called. This should be one of the first things to be done on bootup,
 * since it provides the output sink and the prom version that it used
 * for feedback and debugging purposes.
 */
void prom_init() {

  node_chosen = prom_finddevice("/chosen");
  if (!node_chosen || (int) node_chosen == -1)
    prom_exit();

  if(prom_getprop(node_chosen, "stdout", &node_stdout,
      sizeof(node_stdout)) <= 0)
    node_stdout = 0;

  if(prom_getprop(node_stdin, "stdin", &node_stdin,
      sizeof(node_stdin)) <= 0)
    node_stdin = 0;

  node_root = prom_finddevice("/");
  if (!node_root || (int) node_root == -1) {
    printk("Error: / device not found.\n");
    prom_exit();
  }

  if(prom_getprop(node_root, "compatible", prom_subarch,
    sizeof(prom_subarch)) <= 0) {
    printk("Note: compatible property not set.\n");
    /* not important, we can check compatibility with the %ver register */
    strncpy(prom_subarch, "unknown", 7);
  }

  node_obp = prom_finddevice("/openprom");
  if (!node_obp || (int) node_obp == -1) {
    printk("Error: /openprom device not found.\n");
    prom_exit();
  }

  if(prom_getprop(node_obp, "version", prom_version,
    sizeof(prom_version)) <= 0) {
    printk("Error: /openprom version property not found.\n");
    prom_exit();
  }
  if(strncmp("OBP ", prom_version, 4)) {
    printk("Error: Unknown OpenPROM version.\n");
    prom_exit();
  }

  prom_getprop(node_chosen, "bootpath", prom_bootpath,
               sizeof(prom_bootpath));
  prom_getprop(node_chosen, "bootargs", prom_bootargs,
               sizeof(prom_bootargs));

  /* if we found stdout, we can now clear the screen for the boot messages */
  if(node_stdout != 0)
    clear_screen();
}

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
