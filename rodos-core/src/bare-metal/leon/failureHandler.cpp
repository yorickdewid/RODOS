/**
 * @author Andre Bartke <dev@bartke.cc>
 * 
 * @brief Default failure handler.
 */

#include <rodos.h>
#include "failureHandler.h"
#include "hw_core.h"
#include "hw_asmdef.h"

extern "C" int fflush(FILE *);

// bad trap handler, overridable by the final user
void hwHandleSysPanic(regs_t *reg) {
	// you may override this default behavior

	// we want to continue in case of an unexpected user trap
	// or in case of unimplemented interrupt level traps.
	int tt = (reg->tbr >> TBR_TT_SHIFT & TBR_TT_MASK);
	if(tt > 127 || (tt >= 0x11 && tt <= 0x1f)) {
		// if we return, the bsp tries to continue
		return;
	}

	// by default we just print the trapped register file
	xprintf("System panic: unexpected trap\n");
	xprintf(" --- processor state\n");
	xprintf("   TT: 0x%02x\n", (reg->tbr >> TBR_TT_SHIFT & TBR_TT_MASK));
	xprintf("   PC: 0x%08x\n", reg->pc);
	xprintf("  NPC: 0x%08x\n", reg->npc);
	xprintf("  PSR: 0x%08x\n", reg->psr);
	xprintf("  TBA: 0x%08x\n", (reg->tbr >> TBR_TBA_SHIFT & TBR_TBA_MASK) << TBR_TBA_ADDRSHIFT);
	xprintf(" --- trapped register file\n");
	for(int i=1;i<8;i++)
		xprintf("  %%g%d: 0x%08x\n", i, reg->tf_g[i]);
	for(int i=0;i<8;i++)
		xprintf("  %%i%d: 0x%08x\n", i, reg->tf_i[i]);
	xprintf(" --- register window at which the trap occurred\n");
	for(int i=0;i<8;i++)
		xprintf("  %%l%d: 0x%08x    %%i%d: 0x%08x\n", i, reg->sf_l[i], i, reg->sf_i[i]);
	xprintf("\n");

	// make sure everything is printed
	fflush(0);

	// and enter error mode
	hwErrorMode();
}

/* vim: set noet ci pi sts=0 sw=4 ts=4: */
