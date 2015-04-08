
#ifndef __RODOS_APID__
#define __RODOS_APID__


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/**
	Definition of APIDS: Application Ids  for predefined RODOS 
	and user services

	appliation in RODOS: Ids
	0    ...   99: RODOS internal
	100  ...  999: Input Output application
	1000 ... 2G:  User applications
*/


/************ 0 ... 99: Rodos intern topics **************/

#define APID_ANONYM                 	  0
#define APID_MIDDLEWARE			 10


/************ 100 ... 999:  Input / Output Sercies ***/

#define APID_GATEWAY_GENERIC         	 20
#define APID_GATEWAY_UDP             	 21
#define APID_GATEWAY_UART            	 22


/******************************************************/

#define FIRST_USER_APID  		100 



#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif
