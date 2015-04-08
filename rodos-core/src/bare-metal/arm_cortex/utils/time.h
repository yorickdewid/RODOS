#ifndef TIME_H_
#define TIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

//*****************************************************************************
//
// !This function gets time for file system.
// !This function use RTC from uP
// ! \param none
// ! \return time in date format from RTC
//
//*****************************************************************************
unsigned long get_fattime (void);
//*****************************************************************************
//
// !This function convert binary time to date format.
// !This function use RTC from uP
// ! \param x time in binary format
// ! \param cpTimeDate pointer to string with date
// ! \return time in date format
//
//*****************************************************************************
unsigned long day2date(unsigned long x, char *cpTimeDate);
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
                         unsigned int min, unsigned int sec);

#ifdef __cplusplus
 }
#endif

#endif /*TIME_H_*/