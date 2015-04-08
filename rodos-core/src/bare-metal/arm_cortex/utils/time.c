#include "time.h"
#include "../hw/hw_types.h"
#include "../hw/hw_memmap.h"
#include "../hw/hw_ints.h"
#include "../src/hibernate.h"
#include "../utils/ustdlib.h"

//*****************************************************************************
//
// !This function gets time for file system.
// !This function use RTC from uP
// ! \param none
// ! \return time in date format from RTC
//
//*****************************************************************************
unsigned long get_fattime (void)
{
	unsigned long lData;
	
	lData = HibernateRTCGet();
	return day2date(lData,NULL);
}
//*****************************************************************************
//
// !This function convert binary time to date format.
// !This function use RTC from uP
// ! \param x time in binary format
// ! \param cpTimeDate pointer to string with date
// ! \return time in date format
//
//*****************************************************************************
unsigned long day2date(unsigned long x, char *cpTimeDate)	
{
int	yrs = 99, mon = 99, day = 99, tmp, jday, hrs, min, sec;
unsigned long j, n;

	j = x / 60;				/* whole minutes since 1/1/70 */
	sec = x - (60 * j);		/* leftover seconds */
	n = j / 60;
	min = j - (60 * n);
	j = n / 24;
	hrs = n - (24 * j);

	j = j + (365 + 366);		/* whole days since 1/1/68 */
	day = j / ((4 * 365) + 1);
	tmp = j % ((4 * 365) + 1);

	if(tmp >= (31 + 29))		/* if past 2/29 */
		day++;			/* add a leap day */

	yrs = (j - day) / 365;		/* whole years since 1968 */
	jday = j - (yrs * 365) - day;	/* days since 1/1 of current year */
	if(tmp <= 365 && tmp >= 60)	/* if past 2/29 and a leap year then */
		jday++;			/* add a leap day */
	yrs += 1968;			/* calculate year */

	for(mon = 12; mon > 0; mon--)
	{
		switch(mon)
		{
			case 1:	tmp = 0;	break;
			case 2:	tmp = 31;	break;
			case 3:	tmp = 59;	break;
			case 4:	tmp = 90;	break;
			case 5:	tmp = 120;	break;
			case 6:	tmp = 151;	break;
			case 7:	tmp = 181;	break;
			case 8:	tmp = 212;	break;
			case 9:	tmp = 243;	break;
			case 10:	tmp = 273;	break;
			case 11:	tmp = 304;	break;
			case 12:	tmp = 334;	break;
		}
		if((mon > 2) && !(yrs % 4))	/* adjust for leap year */
			tmp++;
		if(jday >= tmp)	break;
	}
	day = jday - tmp;		/* calculate day in month */
  /* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */
  /* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */
	if(cpTimeDate != NULL)
	{
		usprintf(cpTimeDate, "%04d/%02d/%02d %02d:%02d:%02d", yrs ,mon, day, hrs, min, sec);
		cpTimeDate[19]=0x00;
	}
    n=0;
    j=0;
    n=yrs-1980;
    n=(n<<25)&0xfe000000;
    j=mon; j=j<<21; j=j&0x01e00000;
    n=n|j;
    j=0; j=day; j=j<<16; j=j&0x1f0000;
    n=n|j;
    j=0; j=hrs; j=j<<11; j=j&0xf800;
    n=n|j;
    j=0; j=min; j=j<<5; j=j&0x7e0;
    n=n|j;
    j=0; j=sec; j=j&0x1f;
    n=n|j;
    return n;
}
//*****************************************************************************
//
// !This function convert date to elapsed days in binary
// ! \param yr year
// ! \param mo month
// ! \param da day
// ! \param hrs hours
// ! \param min minutes
// ! \param sec seconds
// ! \return time in binary format
//
//*****************************************************************************
unsigned long	date2day(unsigned int yr,  unsigned int mo,
						 unsigned int da,  unsigned int hrs,
                         unsigned int min, unsigned int sec)
{
unsigned long x;

	/* the following is broken down for clarity */
	x = 365 * (yr - 1970);			/* calculate number of days for previous years */
	x += (yr - 1969) >> 2;			/* add a day for each leap year */
	if((mo > 2) && (yr % 4 == 0))		/* add a day if current year is leap and past Feb 29th */
		x++;

	switch(mo)
	{
		case 1:	x += 0;	break;
		case 2:	x += 31;	break;
		case 3:	x += 59;	break;
		case 4:	x += 90;	break;
		case 5:	x += 120;	break;
		case 6:	x += 151;	break;
		case 7:	x += 181;	break;
		case 8:	x += 212;	break;
		case 9:	x += 243;	break;
		case 10:	x += 273;	break;
		case 11:	x += 304;	break;
		case 12:	x += 334;	break;
	}

	x += da;					/* finally, add the days into the current month */
	x = x * 86400;				/* and calculate the number of seconds in all those days */
	x += (hrs * 1800);			/* add the number of seconds in the hours */
	x += (hrs * 1800);			/* add the number of seconds in the hours */
	x += (min * 60);				/* ditto the minutes */
	x += sec;					/* finally, the seconds */
	return(x);
}
