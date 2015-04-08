
#ifndef ARMv4_ABORT_H
#define ARMv4_ABORT_H

#include <stdint.h>
namespace RODOS {
typedef struct
{
	uint32_t sp_usr;
	uint32_t sp_fiq;
	uint32_t sp_irq;
	uint32_t sp_svc;
	
	uint32_t reg[16];
	uint32_t pc;
	uint32_t abort_id;
	uint32_t cpsr;
	//uint32_t magic_code;
} AbortContext;
}
#endif // ARMv4_ABORT_H
