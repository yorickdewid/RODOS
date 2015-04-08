
#include "utils/time.h"
#include "hw/hw_types.h"
#include "hw/hw_memmap.h"
#include "hw/hw_ints.h"
#include "src/sysctl.h"
#include "src/gpio.h"
#include "src/systick.h"
#include "src/interrupt.h"
#include "src/uart.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "src/ethernet.h"
#include "src/lm3s_flash.h"
#include "src/hibernate.h"

unsigned int tmr1=0;

void setRTC(unsigned int yr,  unsigned int mo,
		    unsigned int da,  unsigned int hrs,
            unsigned int min, unsigned int sec)
{
	HibernateRTCSet(date2day(yr,mo,da,hrs,min,sec));
}
unsigned long getRTC(char *TimeDate)
{
	unsigned long lData;
	
	lData = HibernateRTCGet();
	return day2date(lData,TimeDate);
}
