/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief The OpenBoot PROM client interface.
 * This file contains routines concerning the Sun IEEE 1275 PROM low level
 * interface.
 */

#include <stdarg.h>
#include <oplib.h>
#include <param.h>

/**
 * @brief Perform an OpenBoot PROM call to write to an output sink.
 *
 * @param handle The output sink to write to, in most cases stdout.
 * @param str The C string to be written to the output sink.
 * @param len The length of the C string.
 *
 * @return -1 on failure.
 */
int prom_write(int handle, const char *str, int len) {
  if (len > 1024) /* REALLY? */
    return -1;

  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "write";
  args[1] = 3;
  args[2] = 1;
  args[3] = (uint32_t) handle;
  args[4] = (uint64_t) str;
  args[5] = (uint64_t) len;

  if (prom_call(args) == -1)
    return -1;

  return (int) args[6];
}

/**
 * @brief Perform a OpenBoot PROM call to get a device property.
 *
 * @param device The device that holds the property.
 * @param prop The name of the property.
 * @param buf A buffer where the property will be written to.
 * @param buflen The size of the buffer.
 *
 * @return The size of the property, -1 on failure.
 */
int prom_getprop(int device, char *prop, void *buf, int buflen) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "getprop";
  args[1] = 4;
  args[2] = 1;
  args[3] = (uint32_t) device;
  args[4] = (uint64_t) prop;
  args[5] = (uint64_t) buf;
  args[6] = buflen;

  if (prom_call(args) == -1)
    return -1;

  return (int) args[7];
}

long prom_getproplen(int handle, char *prop) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "getproplen";
  args[1] = 2;
  args[2] = 1;
  args[3] = (uint32_t) handle;
  args[4] = (uint64_t) prop;

  if (prom_call(args) == -1)
    return -1;

  return args[5];
}

/**
 * @brief Issues a call to OpenBoot PROM to get the handle of a device.
 *
 * @param device The name of the device to search fore.
 *
 * @return The device handler on success, -1 on failure.
 */
int prom_finddevice(char *device) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "finddevice";
  args[1] = 1;
  args[2] = 1;
  args[3] = (uint64_t) device;

  if (prom_call(args) == -1)
    return -1;

  return (int) args[4];
}

int prom_getpeer(int device) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "peer";
  args[1] = 1;
  args[2] = 1;
  args[3] = (uint32_t) device;

  if (prom_call(args) == -1)
    return -1;

  return (int) args[4];
}

int prom_getchild(int device) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "child";
  args[1] = 1;
  args[2] = 1;
  args[3] = (uint32_t) device;

  if (prom_call(args) == -1)
    return -1;

  return (int) args[4];
}

int prom_getparent(int device) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "parent";
  args[1] = 1;
  args[2] = 1;
  args[3] = (uint32_t) device;

  if (prom_call(args) == -1)
    return -1;

  return (int) args[4];
}

int prom_setprop(int device, char *prop, const void *buf, int buflen) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "setprop";
  args[1] = 4;
  args[2] = 1;
  args[3] = (uint32_t) device;
  args[4] = (uint64_t) prop;
  args[5] = (uint64_t) buf;
  args[6] = buflen;

  if (prom_call(args) == -1)
    return -1;

  return (int) args[7];
}

void prom_exit() {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "exit";
  args[1] = 0;
  args[2] = 0;

  prom_call(args);
}

void prom_halt() {
  prom_exit();
}

/**
 * @brief Reset and reboot the machine.
 *
 * @param boot_command reboot command "reset-all"
 */
void prom_reboot(const char *boot_command) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "boot";
  args[1] = 1;
  args[2] = 0;
  args[3] = (uint64_t) boot_command;

  (void) prom_call(&args);

  /* For testing purposes, QEMU seems to ignore the boot command so we have
   * to put something extra in here. 2012-03 -AndreB
   */
  args[0] = (uint64_t) "sparc64-reset-all";
  args[1] = 1;
  args[2] = 0;
  args[3] = (uint64_t) boot_command;

  (void) prom_call(&args);
}

int prom_callmethod(char *method, int handle, int nargs, ...) {
  va_list list;
  uint32_t n;

  if(nargs > 6)
    return -1;

  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "call-method";
  args[1] = nargs+2; /* +2 for numin, numout */
  args[2] = 1;
  args[3] = (uint64_t) method;
  args[4] = (uint32_t) handle;

  va_start(list, nargs);
  for(n = 5; n <= args[1]; n++) {
    args[n] = va_arg(list, uint64_t);
  }
  va_end(list);

  if(prom_call(args) == -1)
    return -1;

  /* return value is nargs+3, (service, nargs, nreturns) = 3 */
  return args[args[1]+3];
}

int prom_packagetopath(int handle, const char *buf, int buflen) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "package-to-path";
  args[1] = 3;
  args[2] = 1;
  args[3] = (uint32_t) handle;
  args[4] = (uint64_t) buf;
  args[5] = (uint64_t) buflen;

  if(buflen > PAGE_SIZE)
    return -1;

  if (prom_call(args) == -1)
    return -1;

  return (int) args[6];
}

uint32_t prom_gettime(void) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "milliseconds";
  args[1] = 0;
  args[2] = 1;

  (void) prom_call(args);

  return (uint32_t) args[3];
}

/**
 * @brief Points OpenBoot PROM to our trap table.
 *
 * @param tba Trap table base address.
 *
 * @return -1 on failure.
 */
int prom_settraptable(uint64_t tba) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "SUNW,set-trap-table";
  /* Compatibility note: (sun4v support has yet to be added)
   * The sun4v version has an additional argument for the miss area.
   *
   * if(sun4v)
   *   args[1] = 2;
   * else
   *   args[1] = 1;
   * ...
   * args[4] = (uint64_t) mmfsa;
   */
  args[1] = 1;
  args[2] = 0;
  args[3] = (uint64_t) tba;

  return prom_call(&args);
}

void prom_poweroff() {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "SUNW,power-off";
  args[1] = 0;
  args[2] = 0;

  prom_call(args);
}

#ifdef __SMP__
void prom_startcpu(int cpunode, uint64_t pc, uint64_t o0) {
  uint64_t args[PROM_ARGS_MAX+3];
  args[0] = (uint64_t) "SUNW,start-cpu";
  args[1] = 3;
  args[2] = 0;
  args[3] = (uint32_t) cpunode;
  args[4] = (uint64_t) pc;
  args[5] = (uint64_t) o0;

  (void) prom_call(args);
}
#endif


/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
