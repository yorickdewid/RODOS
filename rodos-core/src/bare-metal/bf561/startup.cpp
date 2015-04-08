
#include "hw_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__dso_handle = 0;

typedef void (constructor_t)();

extern u32  __ctor_start;
extern u32  __ctor_end;

extern u32 _bss_start;
extern u32 _bss_end;
extern u32 _l3_nocache_start;
extern u32 _l3_nocache_end;
//extern constructor_t  *___CTORS_END;

void cpp_constructors() {
#warning check this dtors ctors und so weiter
	u32 *pCtor;
	constructor_t *fn;
	pCtor = &__ctor_start;

	while (pCtor < &__ctor_end) {
		fn =  (constructor_t *) *pCtor;
		(*fn)();
		//*pEBIU_SDBCTL = MFC_EBIU_SDBCTL;

		pCtor++;
	}

}


void chip_init() {
	*pIMEM_CONTROL = LRUPRIORST;
	*pDMEM_CONTROL = 0;
	//sdram init
	//\todo if sdram memory is needed the external bus unit has to be initialized by the second stage loader
//	*pEBIU_SDBCTL = MFC_EBIU_SDBCTL;
//	*pEBIU_SDRRC = MFC_EBIU_SDRRC;
//	*pEBIU_SDGCTL = MFC_EBIU_SDGCTL;
	//*((u32*)0)=0;

	//async mem init
	*pEBIU_AMGCTL = MFC_EBIU_AM_CTL;
	*pEBIU_AMBCTL0 = MFC_EBIU_AMCTL0;
	*pEBIU_AMBCTL1 = MFC_EBIU_AMCTL1;
}


void bss_init() {
	u32 *pRam;

	pRam = &_bss_start;
	while (pRam < &_bss_end) {
		*pRam = 0;
		ssync();
		pRam++;
	}

	pRam = &_l3_nocache_start;
	while (pRam < &_l3_nocache_end) {
		*pRam = 0;
		ssync();
		pRam++;
	}
}

/**
 *
 * \brief The pll is configured in this routine
 * Settings are in the sytem_config header file.
 * Only necessary to run on core A
 *
 * \todo VCO settings have to be set
 * \todo Check if core is in the correct operation mode
 *
 */
void init_pll() {

	//set registers for the pll settings
	*pPLL_DIV = MFC_PLL_DIV_REG;
	*pPLL_CTL = MFC_PLL_CTL_REG;
	*pPLL_LOCKCNT = ~0;

	*pSICA_IWR0 |= (1<<0); //set bit 0 pll wake up

	//before new pll settings can be adapted the core has to go into the idle mode
	asm (
			"cli r0;"
			"idle;"
			"sti r0;"
			);
}

#ifdef __cplusplus
}
#endif
