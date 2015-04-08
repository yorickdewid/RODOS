/*
 * @file MPCore.h
 * @date 2013/07/24 12:00
 * @author Tiago Duarte
 * @description: Multiple Core framework for handling a Multiple-Core environment.
 */

#include "MPCore/MPCore.h"

extern unsigned int _start_secondary;

void start_core1(function_to_run func){
    HW_SRC_GPR3_WR((unsigned int)&_start_secondary);
    HW_SRC_GPR4_WR((unsigned int)func);

    unsigned int val;
    val = HW_SRC_SCR_RD();
    val |= 1 << BP_SRC_SCR_CORE1_RST;
    val |= 1 << BP_SRC_SCR_CORE1_ENABLE;
    HW_SRC_SCR_WR(val);
}

void start_core2(function_to_run func){
    HW_SRC_GPR5_WR((unsigned int)&_start_secondary);
    HW_SRC_GPR6_WR((unsigned int)func);

    unsigned int val;
    val = HW_SRC_SCR_RD();
    val |= 1 << BP_SRC_SCR_CORE2_RST;
    val |= 1 << BP_SRC_SCR_CORE2_ENABLE;
    HW_SRC_SCR_WR(val);
}

void start_core3(function_to_run func){
    HW_SRC_GPR7_WR((unsigned int)&_start_secondary);
    HW_SRC_GPR8_WR((unsigned int)func);

    unsigned int val;
    val = HW_SRC_SCR_RD();
    val |= 1 << BP_SRC_SCR_CORE3_RST;
    val |= 1 << BP_SRC_SCR_CORE3_ENABLE;
    HW_SRC_SCR_WR(val);
}

int start_rodos_core1(){
	extern unsigned int _RODOS_image1;
	unsigned int *pointer = &_RODOS_image1;

	// Read the 1st instruction of the RODOS image
	unsigned int first_instruction = (unsigned int)*pointer;

	// If instruction is valid, then run it
	if((first_instruction & 0xFFFFF000) == 0xE59FF000){
		HW_SRC_GPR3_WR((unsigned int)&_RODOS_image1);
		HW_SRC_GPR4_WR((unsigned int)&_RODOS_image1);

		unsigned int val;
		val = HW_SRC_SCR_RD();
		val |= 1 << BP_SRC_SCR_CORE1_RST;
		val |= 1 << BP_SRC_SCR_CORE1_ENABLE;
		HW_SRC_SCR_WR(val);
		return 1;
	}
	return 0;
}

int start_rodos_core2(){
	extern unsigned int _RODOS_image2;
	unsigned int *pointer = &_RODOS_image2;

	// Read the 1st instruction of the RODOS image
	unsigned int first_instruction = (unsigned int)*pointer;

	// If instruction is valid, then run it
	if((first_instruction & 0xFFFFF000) == 0xE59FF000){
		HW_SRC_GPR5_WR((unsigned int)&_RODOS_image2);
		HW_SRC_GPR6_WR((unsigned int)&_RODOS_image2);

		unsigned int val;
		val = HW_SRC_SCR_RD();
		val |= 1 << BP_SRC_SCR_CORE2_RST;
		val |= 1 << BP_SRC_SCR_CORE2_ENABLE;
		HW_SRC_SCR_WR(val);
		return 1;
	}
	return 0;
}

int start_rodos_core3(){
	extern unsigned int _RODOS_image3;
	unsigned int *pointer = &_RODOS_image3;

	// Read the 1st instruction of the RODOS image
	unsigned int first_instruction = (unsigned int)*pointer;

	// If instruction is valid, then run it
	if((first_instruction & 0xFFFFF000) == 0xE59FF000){
		HW_SRC_GPR7_WR((unsigned int)&_RODOS_image3);
		HW_SRC_GPR8_WR((unsigned int)&_RODOS_image3);

		unsigned int val;
		val = HW_SRC_SCR_RD();
		val |= 1 << BP_SRC_SCR_CORE3_RST;
		val |= 1 << BP_SRC_SCR_CORE3_ENABLE;
		HW_SRC_SCR_WR(val);
		return 1;
	}
	return 0;

}

void reset_core1(){
	unsigned int val;
	val = HW_SRC_SCR_RD();
	val |= 1 << BP_SRC_SCR_CORE1_RST;
	HW_SRC_SCR_WR(val);
}

void reset_core2(){
	unsigned int val;
	val = HW_SRC_SCR_RD();
	val |= 1 << BP_SRC_SCR_CORE2_RST;
	HW_SRC_SCR_WR(val);
}

void reset_core3(){
	unsigned int val;
	val = HW_SRC_SCR_RD();
	val |= 1 << BP_SRC_SCR_CORE3_RST;
	HW_SRC_SCR_WR(val);
}

void disable_core1(){
	HW_SRC_SCR_CLR(BM_SRC_SCR_CORE1_ENABLE);
}

void disable_core2(){
	HW_SRC_SCR_CLR(BM_SRC_SCR_CORE2_ENABLE);
}

void disable_core3(){
	HW_SRC_SCR_CLR(BM_SRC_SCR_CORE3_ENABLE);
}
