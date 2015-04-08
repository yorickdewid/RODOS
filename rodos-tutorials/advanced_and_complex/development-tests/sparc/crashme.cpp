// trigger some unexpected traps on SPARC V8
// author: Andre Bartke <dev@bartke.cc>

/* Example output for a build of
 * crashme.cpp with timewaitfor.cpp

...
After 2 Seconds  :  14.090028000 7
After 2 Seconds  :  16.100027999 8
After 2 Seconds  :  18.110027999 9
trigger 0x91: unimplemented user trap
trigger 0x92: unimplemented user trap
After 2 Seconds  :  20.120028000 10
After 2 Seconds  :  22.130027999 11
trigger 0x02: unimplemented instruction
System panic: unexpected trap
 --- processor state
   TT: 0x02
   PC: 0x40007580
  NPC: 0x40007584
  PSR: 0xF3903081
  TBA: 0x04000000
 --- trapped register file
  %g1: 0x00000161
  %g2: 0x80000100
  %g3: 0x00000160
  %g4: 0x00000000
  %g5: 0x00000000
  %g6: 0x997CCB28
  %g7: 0x40007400
  %i0: 0x40007400
  %i1: 0x98DCF148
  %i2: 0x00000000
  %i3: 0xA817C800
  %i4: 0x00000000
  %i5: 0x00000000
  %i6: 0x4000D908
  %i7: 0x4000708C
 --- register window at which the trap occurred
  %l0: 0x00000001    %i0: 0x400088A8
  %l1: 0x40007798    %i1: 0x00000064
  %l2: 0x00000000    %i2: 0x00000000
  %l3: 0x77359400    %i3: 0x00FFFFFF
  %l4: 0x400077B8    %i4: 0x00FFCE31
  %l5: 0x00000000    %i5: 0x80000340
  %l6: 0x00000000    %i6: 0x4000D968
  %l7: 0x00000000    %i7: 0x40002FB0

[ ERROR MODE ]
*/

#include <rodos.h>

// some garbage
const char inst[] = {
	"\xf4\xf4\xf4\xf4"
};

class CrashMe : public Thread {
public:
	CrashMe () : Thread("crashme: Crashes the system") {}

	virtual void run() {
		// wait a bit before we trigger the panic
		suspendCallerUntil(NOW() + 20000*MILLISECONDS);

		// some examples

		// software traps are ignored by default
		xprintf("trigger 0x91: unimplemented user trap\n");
		__asm volatile("ta 17\n");
		xprintf("trigger 0x92: unimplemented user trap\n");
		__asm volatile("ta 18\n");

		// show that we can continue
		suspendCallerUntil(NOW() + 4000*MILLISECONDS);

		// for standard SPARC exception traps we enter error mode by default
		xprintf("trigger 0x02: unimplemented instruction\n");
		__asm volatile("jmpl %0, %%g0\n"
		               :
		               : "r" (inst));

		xprintf("trigger 0x07: memory address not aligned exception\n");
		__asm volatile("st %%g0, [%0 + 1]\n"
		               :
		               : "r" (inst));
	}
} crashme;

/* vim: set noet ci pi sts=0 sw=4 ts=4: */
