
#ifndef __MONITORING_GLOBALS_H__
#define __MONITORING_GLOBALS_H__

/********************* Tasks *********************/

#define TASK_MONITOR		1

#define TASK_NEGCNT		3
#define TASK_ODDCNT	 	4
#define TASK_EVENCNT 		5
#define TASK_PRIMCNT		6
#define TASK_ODDEVENCNT		6
#define TASK_MONITOR_GUI	7


enum monitorMsgTypes {
	NULL_MSG,
	NEG_CNT,
	ODD_CNT,
	EVEN_CNT_PRE,
	EVEN_CNT_POST,
	PRIM_CNT_PRE,
	PRIM_CNT_POST,
	ODDEVEN_CNT
};


#endif
