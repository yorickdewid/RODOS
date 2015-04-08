/**
 * @file cache.h
 * @author Andre Bartke <dev@bartke.cc>, 
 *         Tobias Mikschl <tobias.mikschl@uni-wuerzburg.de>
 * 
 * @brief PPC405 Cache Control.
 */

#ifndef PPC405_CACHE_H
#define PPC405_CACHE_H

#define DCACHE_RAM_SIZE     16384
#define CACHELINE_SIZE      32

#include <registers.h>

static inline void icache_disable() {
  __asm volatile(
      "iccci  0, 0\n" // invalidate icache
      "mticcr %0\n"   // icache control register
      "isync\n"       // flush
      :
      : "r" (0));
}

static inline void dcache_disable() {
  uint32_t addr = 0;
  uint32_t msr = 0;
  
  // disable EE and CE to avoid memory corruption
  __asm volatile(
      "mfmsr %0\n"
      "andn  %1, %1, %2\n"
      "mtmsr %1\n"
      : "=r" (msr)
      : "r" (msr), "r" (MSR_EE|MSR_CE));

  // flush dcache
  while(addr < (DCACHE_RAM_SIZE/2)) {
    // iterate through the number congruence classes
    __asm volatile(
        "dcbz  0, %0\n"  // flush one way
        "dcbz  %1, %0\n" // flush the other way
        "dccci 0, %0\n"  // invalidate dcache set
        :
        : "r" (addr), "r" (DCACHE_RAM_SIZE));
    // point to the next congruence class
    addr += CACHELINE_SIZE;
  }
  
  // disable dcache
  __asm volatile(
      "sync\n"      // make sure cache line is written
      "mtdccr %0\n" // dcache control register
      :
      : "r" (0));
  
  // restore MSR
  __asm volatile(
      "mtmsr %0\n"
      :
      : "r" (msr));
}

/* The storage-attribute control-registers are 32-bit registers. Each bit is
 * associated with a 128 MB memory region: bit 0 controls the lowest 128 MB
 * region, bit 1 controls the nextlowest 128 MB region, and so on. Together,
 * the 32 register bits provide storage control across the entire 4 GB
 * physical-address space.
 *
 * Address range example:
 * bit 0: 0x0000_0000 to 0x07FF_FFFF
 * bit 1: 0x0800_0000 to 0x0FFF_FFFF
 */

static inline void icache_enable(uint32_t mask) {
  __asm volatile(
      "mtsgr  %0\n" // disable possible storage guards
      "iccci  0, 0\n"
      "mticcr %1\n"
      "isync\n"
      :
      : "r" (0), "r" (mask));
}

static inline void dcache_enable(uint32_t mask) {
  uint32_t addr = 0;
  
  // invalidate dcache
  while(addr < (DCACHE_RAM_SIZE/2)) {
    // iterate through the number congruence classes
    __asm volatile(
        "dccci 0, %0\n" // invalidate dcache
        :
        : "r" (addr));
    // point to the next congruence class
    addr += CACHELINE_SIZE;
  }

  // disable dcache
  __asm volatile(
      "mtsgr  %0\n"
      "mtdcwr %1\n" // dcache write policy: write-through
      "mtdccr %2\n" // dcache control register
      "isync\n"
      :
      : "r" (0), "r" (1), "r" (mask));
}

#endif // PPC405_CACHE_H

