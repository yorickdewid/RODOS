/*
 * Tiago Duarte 10-05-2013
 * This file reads the function pointer of the constructors and execute them.
 * This pointers are located in the .ctors section and they are the .init_array
 * section.
 * Also enable the caches and the branch prediction. The cache and branch prediction
 * improve the overall system performance. To test the cache
 * and branch impact in the system execute cpuspeedtest.cpp with and without enabling
 * caches and branch prediction.
 * Results:
 * 		- K-loops/sec with caches and branch: 2341
 * 		- K-loops/sec without caches and branch: 1733
 */
#include "workspace.h"
#include "rodos.h"

extern "C" {

// Symbols defined in the linker script
extern void (*__CTOR_LIST__)();
extern void (*__init_array_start)();
extern char __l1_page_table_start;

void constructors_init(){
	/*
	 * .ctors section
	 */
	void (**ctor)(void) = &__CTOR_LIST__;
	int entries = *(unsigned int *)ctor++;
	// call all global constructors
	while(entries--){
		(*ctor++)();
	}

	/*
	 * .init_array section
	 */
	void (**init_arry)(void) = &__init_array_start;
	entries = *(unsigned int *)init_arry++;
	// call all global constructors
	while(entries--){
		(*init_arry++)();
	}
}

void init_memory_system(){

	if(cpu_get_current() !=0){
		scu_join_smp();
		scu_enable_maintenance_broadcast();
	}


	/****************************************
	 * MMU
	 */
	// Disable MMU
	mmu_disable();

	// Initiate MMU - initiate the peripherals
	mmu_init();

	// Enable MMU
	mmu_enable();


	/****************************************
	 * Branch prediction
	 */
	// Disable branch prediction
	arm_branch_prediction_disable();

	// Invalidate branch prediction array
	arm_branch_target_cache_invalidate();

	// Branch Prediction Enable
	arm_branch_prediction_enable();

	/****************************************
	 * Data Cache
	 */
	// Disable L1 Data Caches
	arm_dcache_disable();

	// Invalidate Data cache
	arm_dcache_invalidate();

	// Enable Data cache
	arm_dcache_enable();

	/****************************************
	 * Instruction Cache
	 */
	// Disable L1 Instruction cache
	arm_icache_disable();

	// Invalidate Instruction cache
	arm_icache_invalidate();

	// Enable Instruction cache
	arm_icache_enable();

	/****************************************
	 * L2 Cache
	 */
	if(cpu_get_current() == 0){
		// Disable L2
		_l2c310_cache_disable();

		// Set up L2 cache
		_l2c310_cache_setup();

		// Invalidate L2 cache
		_l2c310_cache_invalidate();

		// Enable L2 cache
		_l2c310_cache_enable();


		scu_enable();
		scu_join_smp();
	}



}

void init_memory_secondary(){
	// Disable MMU
	mmu_disable();

	// Get the L1 page table base address.
	uint32_t * table = (uint32_t *)&__l1_page_table_start;

	// write entry table address to TTBR0
	_ARM_MCR(15, 0, table, 2, 0, 0);

	// set Client mode for all Domains
	uint32_t dacr = 0x55555555;
	_ARM_MCR(15, 0, dacr, 3, 0, 0); // MCR p15, 0, <Rd>, c3, c0, 0 ; Write DACR

	// Enable MMU
	mmu_enable();

	/****************************************
	 * Branch prediction
	 */
	// Disable branch prediction
	arm_branch_prediction_disable();

	// Invalidate branch prediction array
	arm_branch_target_cache_invalidate();

	// Branch Prediction Enable
	arm_branch_prediction_enable();

	/****************************************
	 * Instruction Cache
	 */
	// Disable L1 Instruction cache
	arm_icache_disable();

	// Invalidate Instruction cache
	arm_icache_invalidate();

	// Enable Instruction cache
	arm_icache_enable();

	/****************************************
	 * Data Cache
	 */
	// Disable L1 Data Caches
	arm_dcache_disable();

	// Invalidate Data cache
	arm_dcache_invalidate();

	// Enable Data cache
	arm_dcache_enable();
}
}
