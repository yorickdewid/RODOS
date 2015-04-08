/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Architecture specific parameters and constants.
 */


#ifndef _SPARC64_PARAM_H
#define _SPARC64_PARAM_H


/** Round up address A to match the alignment B. */
#define ALIGN_CEIL(A, B)        ((A + (B - 1)) & ~(B - 1))
/** Round down address A to match the alignment B. */
#define ALIGN_FLOOR(A, B)       (A & ~(B - 1))


/**
 * @defgroup hwArchIDs SPARC64 Vendor and architecture IDs.
 */
/**@{*/

/** Vendor id for UltraSPARC I and II */
#define HW_VENDOR_ULTRASPARC    0x17
/** Vendor id for UltraSPARC III and later */
#define HW_VENDOR_SUN           0x3e
/** Vendor id for FUJITSU (just in case) */
#define HW_VENDOR_FUJITSU       0x04

#define HW_ARCH_SPARC64         0x01 /**< SPARC64           */
#define HW_ARCH_SPARC64_II      0x02 /**< SPARC64-II        */
#define HW_ARCH_SPARC64_III     0x03 /**< SPARC64-III       */
#define HW_ARCH_SPARC64_IV      0x04 /**< SPARC64-IV        */
#define HW_ARCH_ZEUS            0x05 /**< SPARC64-V         */
#define HW_ARCH_OLYMPUS_C       0x06 /**< SPARC64-VI        */
#define HW_ARCH_JUPITER         0x07 /**< SPARC64-VII       */
#define HW_ARCH_SPITFIRE        0x10 /**< UltraSPARC        */
#define HW_ARCH_BLACKBIRD       0x11 /**< UltraSPARC-II     */
#define HW_ARCH_SABRE           0x12 /**< UltraSPARC-IIi    */
#define HW_ARCH_HUMMINGBIRD     0x13 /**< UltraSPARC-IIe    */
#define HW_ARCH_CHEETAH         0x14 /**< UltraSPARC-III    */
#define HW_ARCH_CHEETAH_PLUS    0x15 /**< UltraSPARC-III+   */
#define HW_ARCH_JALAPENO        0x16 /**< UltraSPARC-IIIi   */
#define HW_ARCH_JAGUAR          0x18 /**< UltraSPARC-IV     */
#define HW_ARCH_PANTHER         0x19 /**< UltraSPARC-IV+    */
#define HW_ARCH_SERRANO         0x22 /**< UltraSPARC-IIIi+  */

/**@}*/


/**
 * @defgroup hwOutputSections Output Sections.
 */
/**@{*/

/** Virtual address where the kernel starts execution. */
#define VMA                     0x400000

/** Virtual address where the kernel is loaded. */
#define LMA                     0x4000

/**@}*/


/**
 * @defgroup hwArch Architecture specific parameters.
 */
/**@{*/

/** Number of register window sets. */
#define NWINDOWS                8

/** Set max. priority interrupt level. */
#define MAX_IRQ_LEVEL           15

/**@}*/


/**
 * @defgroup hwStack Constants defining a stack frame.
 */
/**@{*/

/*
 * The SPARCv9 ABI defined stack frame:
 *
 *  |                               |
 *  +-------------------------------+
 *  |  locals, saved floats, ...    |
 *  +-------------------------------+
 *  |  past 6 outgoing params       |
 *  +-------------------------------+---- STACK_FRAME_SZ
 *  |  curr 6 outgoing params       |           |
 *  +-------------------------------+           |
 *  |  16 extended words to save    |           |
 *  |  ins and locals on overflow   |           /
 *  +-------------------------------+---- STACK_BIAS
 *  |                               |           |
 *  |                               |           |
 *  |                               |           /
 *  +-------------------------------+ <- %sp
 */

/**
 * The SPARC64 ABI dictates a stack bias of -0x7ff. The odd number will set
 * the least significant bit of the stack pointer what makes it easy for trap
 * handlers to distinguish between 64 bit and 32 bit register windows.
 * Whenever we issue a load or store to a stack location we need to add in
 * the appropriate stack bias or we will get a 'memory address not aligned'
 * exception.
 * This way the actual top of the stack is located at %sp + STACK_BIAS.
 */
#define STACK_BIAS              2047
/** A single extended word is 8-bytes in size. */
#define STACK_ITEM_SZ           8

/** Initial stack size for the bootstrapping process (16k). */
#define STACK_BOOT_SIZE         (PAGE_SIZE*20)

/** 16 extended words ins and locals */
#define STACK_WINDOW_SZ         (STACK_ITEM_SZ * 16)
/** 6 extended words params */
#define STACK_ARGAREA_SZ        (STACK_ITEM_SZ * 6)

/**
 * Size of a stack frame is at least 176 bytes. 128 bytes for spilling
 * register windows (16 extended words) and 48 bytes to store at least
 * 6 arguments to callees.
 */
#define STACK_FRAME_SZ          176

/** According to the specs, a stack frame must be 16-byte aligned. */
#define STACK_FRAME_ALIGNMENT   16

/**@}*/


/**
 * @defgroup hwMMU Memory management settings.
 */
/**@{*/

/** 64K / 16-bytes per-TSB-entry = 4096 entries (2048 DTSB and 2048 ITSB) */
#define TSB_SIZE_BYTES          (64 * 1024)

/** The page size is 8192 bytes on sun4u */
#define PAGE_SIZE               8192
#define PAGE_SHIFT              13
#define PAGE_MASK               (PAGE_SIZE-1)
#define PAGE_OFFSET             0xFFFFF80000000000

/**@}*/


#endif /* _SPARC64_PARAM_H */

/* vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent: */
