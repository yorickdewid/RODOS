
#ifndef __RODOS_SERVICES_ID_
#define __RODOS_SERVICES_ID_


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/**
	Definition of Topic IDs for predefined RODOS services
	and user services

	services in RODOS: Topic Ids
	0    ...   99: RODOS internal
	100  ...  999: Input Output Services
	1000 ... 64K:  User Services: User defined Topic Ids
*/


/************ 0 ... 99: Rodos intern topics **************/

#define TOPIC_ID_SUBSCRIBERS_REPORT 	0
#define TOPIC_ID_DEF_GATEWAY        	0
#define TOPIC_ID_NETWORK_REPORT	 	1

#define TOPIC_ID_TAKS_DISTRIBUTION	2
#define TOPIC_ID_MONITORING_MSG		3
#define TOPIC_ID_DEBUG_CMD_MSG		4


/************ 100 ... 999:  Input / Output Sercies ***/

#define TOPICID_UART_CONF 		110
#define TOPICID_UART_IN			111
#define TOPICID_UART_OUT		112

#define TOPIC_ID_PWM_CONF		120
#define TOPIC_ID_PWM_WIDTH		121

#define TOPIC_ID_ANALOG_CONF		123
#define TOPIC_ID_ANALOG_CHANS		124	




/******************************************************/

#define FIRST_USER_TOPIC_ID 		1000 


#ifndef NO_RODOS_NAMESPACE
}
#endif


#endif
