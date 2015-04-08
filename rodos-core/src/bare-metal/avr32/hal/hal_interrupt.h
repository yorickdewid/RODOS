/*
 * Copyright (c) 2012 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Sergio Montenegro, Michael Ruffer
 */

#ifndef HAL_INTERRUPT_H_
#define HAL_INTERRUPT_H_

#include <avr32/io.h>

/* AVR32UC3A0512
 * - there are 20 interrupt groups (0...19): see "Interrupt Request Signal Map" p.41
 * - group 1 (EIC) is divided in 11 interrupt lines
 * - group 2 (GPIO) is divided in 14 interrupt lines
 * - group 3 (PDMA) is divided in 15 interrupt lines
 * - group 4 (TC) is divided in 3 interrupt lines
 * - all other groups are not divided: there is only one interrupt line per interrupt group
 * -> over all there are 16+11+14+15+3=59 different interrupt lines you can assign an interrupt handler to
 *
 * - you can assign one of 4 different priorities to each group
 * - interrupts with same priority can be masked with HW_maskInterrupts()
 *
 * The only thing you must be aware of is, that you can not set different interrupt priorities to lines
 * of the same group. Of cause this only affects group 1,2 and 3.
 */

typedef enum {
	IRQ_PRIO_000 = 0,				// AVR32_INTC_INT0
	IRQ_PRIO_001,					// AVR32_INTC_INT1
	IRQ_PRIO_002,					// AVR32_INTC_INT2
	IRQ_PRIO_003					// AVR32_INTC_INT3
} IRQ_PRIO;

typedef enum {
	IRQ_IDX_000 = 0,				// grp 0: System Block Compare
	IRQ_IDX_001 = AVR32_EIC_IRQ_0,	// grp 1: EIC 0
	IRQ_IDX_002 = AVR32_EIC_IRQ_1,	// grp 1: EIC 1
	IRQ_IDX_003 = AVR32_EIC_IRQ_2,	// grp 1: EIC 2
	IRQ_IDX_004 = AVR32_EIC_IRQ_3,	// grp 1: EIC 3
	IRQ_IDX_005 = AVR32_EIC_IRQ_4,	// grp 1: EIC 4
	IRQ_IDX_006 = AVR32_EIC_IRQ_5,	// grp 1: EIC 5
	IRQ_IDX_007 = AVR32_EIC_IRQ_6,	// grp 1: EIC 6
	IRQ_IDX_008 = AVR32_EIC_IRQ_7,	// grp 1: EIC 7
	IRQ_IDX_009 = AVR32_RTC_IRQ,	// grp 1: RTC
	IRQ_IDX_010 = AVR32_PM_IRQ,		// grp 1: Power Manager
	IRQ_IDX_011 = AVR32_FREQM_IRQ,	// grp 1: Frequency Meter
	IRQ_IDX_012 = AVR32_GPIO_IRQ_0, // grp 2: GPIO_INT0 (pin
	IRQ_IDX_013 = AVR32_GPIO_IRQ_1,	// grp 2: GPIO_INT1 (pin
	IRQ_IDX_014 = AVR32_GPIO_IRQ_2,	// grp 2: GPIO_INT2 (pin
	IRQ_IDX_015 = AVR32_GPIO_IRQ_3,	// grp 2: GPIO_INT3 (pin
	IRQ_IDX_016 = AVR32_GPIO_IRQ_4,	// grp 2: GPIO_INT4 (pin
	IRQ_IDX_017 = AVR32_GPIO_IRQ_5,	// grp 2: GPIO_INT5 (pin
	IRQ_IDX_018 = AVR32_GPIO_IRQ_6,	// grp 2: GPIO_INT6 (pin
	IRQ_IDX_019 = AVR32_GPIO_IRQ_7,	// grp 2: GPIO_INT7 (pin
	IRQ_IDX_020 = AVR32_GPIO_IRQ_8,	// grp 2: GPIO_INT8 (pin
	IRQ_IDX_021 = AVR32_GPIO_IRQ_9,	// grp 2: GPIO_INT9 (pin
	IRQ_IDX_022 = AVR32_GPIO_IRQ_10,// grp 2: GPIO_INT10 (pin
	IRQ_IDX_023 = AVR32_GPIO_IRQ_11,// grp 2: GPIO_INT11 (pin
	IRQ_IDX_024 = AVR32_GPIO_IRQ_12,// grp 2: GPIO_INT12 (pin
	IRQ_IDX_025 = AVR32_GPIO_IRQ_13,// grp 2: GPIO_INT13 (pin
	IRQ_IDX_026 = AVR32_PDCA_IRQ_0,	// grp 3: PDMA 0
	IRQ_IDX_027 = AVR32_PDCA_IRQ_1,	// grp 3: PDMA 1
	IRQ_IDX_028 = AVR32_PDCA_IRQ_2,	// grp 3: PDMA 2
	IRQ_IDX_029 = AVR32_PDCA_IRQ_3,	// grp 3: PDMA 3
	IRQ_IDX_030 = AVR32_PDCA_IRQ_4,	// grp 3: PDMA 4
	IRQ_IDX_031 = AVR32_PDCA_IRQ_5,	// grp 3: PDMA 5
	IRQ_IDX_032 = AVR32_PDCA_IRQ_6,	// grp 3: PDMA 6
	IRQ_IDX_033 = AVR32_PDCA_IRQ_7,	// grp 3: PDMA 7
	IRQ_IDX_034 = AVR32_PDCA_IRQ_8,	// grp 3: PDMA 8
	IRQ_IDX_035 = AVR32_PDCA_IRQ_9,	// grp 3: PDMA 9
	IRQ_IDX_036 = AVR32_PDCA_IRQ_10,// grp 3: PDMA 10
	IRQ_IDX_037 = AVR32_PDCA_IRQ_11,// grp 3: PDMA 11
	IRQ_IDX_038 = AVR32_PDCA_IRQ_12,// grp 3: PDMA 12
	IRQ_IDX_039 = AVR32_PDCA_IRQ_13,// grp 3: PDMA 13
	IRQ_IDX_040 = AVR32_PDCA_IRQ_14,// grp 3: PDMA 14
	IRQ_IDX_041 = AVR32_FLASHC_IRQ,	// grp 4: Flash Contr.
	IRQ_IDX_042 = AVR32_USART0_IRQ,	// grp 5: USART 0
	IRQ_IDX_043 = AVR32_USART1_IRQ,	// grp 6: USART 1
	IRQ_IDX_044 = AVR32_USART2_IRQ,	// grp 7: USART 2
	IRQ_IDX_045 = AVR32_USART3_IRQ,	// grp 8: USART 3
	IRQ_IDX_046 = AVR32_SPI0_IRQ,	// grp 9: SPI 0
	IRQ_IDX_047 = AVR32_SPI1_IRQ, 	// grp 10: SPI 1
	IRQ_IDX_048 = AVR32_TWI_IRQ,  	// grp 11: TWI (I2C)
	IRQ_IDX_049 = AVR32_PWM_IRQ, 	// grp 12: PWM
	IRQ_IDX_050 = AVR32_SSC_IRQ, 	// grp 13: SSC
	IRQ_IDX_051 = AVR32_TC_IRQ0, 	// grp 14: Timer
	IRQ_IDX_052 = AVR32_TC_IRQ1, 	// grp 14: Timer
	IRQ_IDX_053 = AVR32_TC_IRQ2, 	// grp 14: Timer
	IRQ_IDX_054 = AVR32_ADC_IRQ,	// grp 15: ADC
	IRQ_IDX_055 = AVR32_MACB_IRQ, 	// grp 16: Ethernet MAC
	IRQ_IDX_056 = AVR32_USBB_IRQ, 	// grp 17: USB
	IRQ_IDX_057 = AVR32_SDRAMC_SDRAMC_IRQ, 	// grp 18: SDRAM Contr.
	IRQ_IDX_058	= AVR32_ABDAC_IRQ 	// grp 19: Audio DAC
} IRQ_IDX;

void HW_setInterruptHandler(IRQ_IDX index, IRQ_PRIO priority, void (*interruptHandler)());

void HW_enableInterrupts();
void HW_disableInterrupts();

void HW_maskInterrupts(IRQ_PRIO prio);
void HW_unmaskInterrupts(IRQ_PRIO prio);

unsigned long HW_getUnhandledInterrupCounter();


#endif /* HAL_INTERRUPT_H_ */
