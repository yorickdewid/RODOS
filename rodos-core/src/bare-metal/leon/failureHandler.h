/**
 * @author Andre Bartke <dev@bartke.cc>
 * 
 * @brief Failure handler.
 */

#ifndef _FAILUREHANDLER_H
#define _FAILUREHANDLER_H

#include <stdint.h>

extern "C" {

// 36*4-bytes
typedef struct {
	// trap frame register
	uint32_t  psr;
	uint32_t  pc;
	uint32_t  npc;
	uint32_t  tf_g[7];
	uint32_t  tf_i[8];
	// stack frame register
	uint32_t  sf_l[8];
	uint32_t  sf_i[8];
	uint32_t  tbr;
} regs_t;

void hwHandleSysPanic(regs_t *reg) __attribute__((weak));

} // extern "C"

#endif // _FAILUREHANDLER_H

/* vim: set noet ci pi sts=0 sw=4 ts=4: */
