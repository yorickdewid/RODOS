/**
 * @file context.h
 * @author Tobias Mikschl <tobias.mikschl@uni-wuerzburg.de>
 *
 * @brief SaveContext and SwitchToContext functions for the Scheduler.
 */


/* context data on the stack
 * [PowerPC System V Application Binary Interface]
 *
 * +164 <- (PC)
 * +160 <- (previous thread stack pointer) <- thread stack pointer
 *  ... <- (padding: not used)
 * +152 <- (pointer to thread data) ;; removed -AndreB
 * +148 <- (CTR)
 * +144 <- (LR)
 * +140 <- (XER)
 * +136 <- (CR)
 * +132 <- (GRPR31)
 *  ... <- (GPR30..3)
 *  +16 <- (GRP2)
 *  +12 <- (GRP1: same as in +0)
 *   +8 <- (GRP0)
 *   +4 <- (schedulerWrapper return address)
 *   +0 <- (thread stack pointer: same as +160) <- kernel stack pointer
 */

#define SAVE_CONTEXT_TO_STACK                       \
    stwu 1, -160(1) ;/* update the stack pointer */ \
                                                    \
    stmw 2, +16(1) ;/* GPR2..31 */                  \
    stw 0, +8(1) ;/* GRP0 */                        \
    la 0, +160(1);                                  \
    stw 0, +12(1)  ; /*GPR1*/                       \
    mfcr 31 ;                                       \
    stw 31, +136(1) ;/* CR */                       \
    mfxer 31 ;                                      \
    stw 31, +140(1) ;/* XER */                      \
    mflr 31  ;                                      \
    stw 31, +144(1) ;/* LR */                       \
    mfctr 31 ;                                      \
    stw 31, +148(1) ;/* CTR*/                       \


#define LOAD_CONTEXT_FROM_STACK                     \
    lwz 0, +148(1) ;                                \
    mtctr 0 ;/* CTR */                              \
    lwz 0, +144(1) ;                                \
    mtlr 0 ;/* LR */                                \
    lwz 0, +140(1) ;                                \
    mtxer 0 ;/* XER */                              \
    lwz 0, +136(1) ;                                \
    mtcr 0 ;/* CR */                                \
                                                    \
    lmw 2, +16(1) ;/* GPR2..31 */                   \
    lwz 0, +8(1) ;/* GPR0 */                        \
    lwz 1, +12(1) ;  /* GPR1 */                     \


