#ifndef _ADC_H_
#define _ADC_H_

/*
 *
 * @author TW
 */

#ifdef __cplusplus
extern "C" {
#endif

void adc_init(void);

int adc_getvalue(int adc_chan);


#ifdef __cplusplus
}
#endif

#endif // _ADC_H_
