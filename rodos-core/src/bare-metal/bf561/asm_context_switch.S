//#include <scheduler.h>
.text;
.file "asm_context_switch.S"
.align 4;
.global ___asmSaveContextAndCallScheduler;
.global ___asmSwitchToContext;
.global ___asmSaveContextAndCallScheduler_isr;
//.extern schedulerWrapper


// maybe with interrupt mask to get in supvisor mode
___asmSaveContextAndCallScheduler:
	//\todo jeck if interrupt is processed
	RAISE 14;
	RTS;
	JUMP(inf_loop);
___asmSaveContextAndCallScheduler_isr:
	[ -- SP ] = RETS;
	[ -- SP ] = ASTAT;
	[ -- SP ] = RETI; 				//PC is storet in RTI

	[ -- SP ] = LC0;
	[ -- SP ] = LC1;

	[ -- SP ] = LT0;
	[ -- SP ] = LT1;

	[ -- SP ] = LB0;
	[ -- SP ] = LB1;				//zählt die Reihenfolge wirklich

	[ -- SP ] = FP;				//zählt die Reihenfolge wirklich

	[ -- SP ] = CYCLES;
	[ -- SP ] = CYCLES2;

	[ -- SP ] = (R7:0 , P5:0);


	[ -- SP ] = A0.W;
	[ -- SP ] = A0.x;
	[ -- SP ] = A1.W;
	[ -- SP ] = A1.x;

	[ -- SP ] = I0;
	[ -- SP ] = I1;
	[ -- SP ] = I2;
	[ -- SP ] = I3;

	[ -- SP ] = B0;
	[ -- SP ] = B1;
	[ -- SP ] = B2;
	[ -- SP ] = B3;

	[ -- SP ] = L0;
	[ -- SP ] = L1;
	[ -- SP ] = L2;
	[ -- SP ] = L3;

	[ -- SP ] = M0;
	[ -- SP ] = M1;
	[ -- SP ] = M2;
	[ -- SP ] = M3;

	//zählt die Reihenfolge wirklich
	R0 = SP;
	call _schedulerWrapper;
inf_loop: JUMP(inf_loop);//should never return from wrapper, context switch, verify




___asmSwitchToContext:
	SP = R0;


	M3 = [SP ++];
	M2 = [SP ++];
	M1 = [SP ++];
	M0 = [SP ++];

	L3 = [SP ++];
	L2 = [SP ++];
	L1 = [SP ++];
	L0 = [SP ++];

	B3 = [SP ++];
	B2 = [SP ++];
	B1 = [SP ++];
	B0 = [SP ++];

	I3 = [SP ++];
	I2 = [SP ++];
	I1 = [SP ++];
	I0 = [SP ++];

	A1.x = [SP ++];
	A1.w = [SP ++];
	A0.x = [SP ++];
	A0.W = [SP ++];

	(R7:0, P5:0) = [SP++]

	CYCLES2 = [SP ++];
	CYCLES = [SP ++];

	FP = [SP ++];

	LB1 = [SP ++];
	LB0 = [SP ++];

	LT1 = [SP ++];
	LT0 = [SP ++];

	LC1 = [SP ++];
	LC0 = [SP ++];

	RETI = [SP ++];
	ASTAT = [SP ++];
	RETS = [SP ++];
	RTI;
	JUMP(inf_loop);
