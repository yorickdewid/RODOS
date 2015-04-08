
#pragma once

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/**************************************************/

/** MessageTypeId end EOM is required just to be able
 	to signalize the End Of Mesage ussing the operator <<
	else they have no use.
 **/

struct MessageTypeId    { int id; };
extern MessageTypeId EOM(int id);


/*** Montor messages: used for both:
	interchange  monitored data (from application to monitor/debugger)
	interchagne  debug messages (from monitor/debugger to applications)
 **/

class MonitoringMsg {
public:
    long long sendTime;	///< Set when the message is sent


    unsigned short msgType;	///< to identify differnt types of monitor messages, defined by user

    char nodeNr; 		///< node of the sending task (monitor msgs) or target task (command msgs)
    unsigned char taskId;
    unsigned char numOfParamsToSend; 	   ///< not all will be send to safe band width and cpu time
    unsigned char numOfParamsDeserialized; ///< only a help variable for the reciever side

    static const int MAX_PARAMS = 64;
    char params[MAX_PARAMS];
    /*** WARNING: Do not add any data field beyond this point, Params has to be the last one **/

    unsigned int numOfBytesToSend() {
	return (int)(params - (char*)this) + numOfParamsToSend;
    }



    /**************************************************************************************/

    MonitoringMsg(int taskId);

    void clear();

    /***********************************/
    /** For the monitored/sender side **/
    /***********************************/

    bool serialize(void* src, int len);	///< append data to the message
    void report(int id);		///< distribute the message

    // These functions could implement byte-sex (littel/big-endian) convertions

    inline bool serialize(char src) 		{ return serialize(&src, sizeof(src)); }
    inline bool serialize(short src) 		{ return serialize(&src, sizeof(src)); }
    inline bool serialize(int src) 		{ return serialize(&src, sizeof(src)); }
    inline bool serialize(long src) 		{ return serialize(&src, sizeof(src)); }
    inline bool serialize(long long src)	{ return serialize(&src, sizeof(src)); }
    inline bool serialize(float src) 		{ return serialize(&src, sizeof(src)); }
    inline bool serialize(double src) 		{ return serialize(&src, sizeof(src)); }

    inline bool serializeString(char* s)	{
        return serialize(s, (int)strlen(s) +1);    // +1: include the termnating 0
    }


    /*******************************************/
    /** For the monitor/receiver side   ********/
    /*******************************************/

    bool deserialize(void* dest, int len);
    void* deserializeNoCopy(int len);

    // could implement byte-sex (littel/big-endian) convertions

    inline bool deserialize(char *a)		{ return deserialize(a, sizeof(*a)); }
    inline bool deserialize(short *a)		{ return deserialize(a, sizeof(*a)); }
    inline bool deserialize(int *a)		{ return deserialize(a, sizeof(*a)); }
    inline bool deserialize(long *a)		{ return deserialize(a, sizeof(*a)); }
    inline bool deserialize(long long *a)	{ return deserialize(a, sizeof(*a)); }
    inline bool deserialize(float *a)		{ return deserialize(a, sizeof(*a)); }
    inline bool deserialize(double *a)		{ return deserialize(a, sizeof(*a)); }

    inline bool deserializeString(char* s)	{
        return deserialize(s, (int)strlen(&params[numOfParamsDeserialized]) +1);    // +1: include the termnating 0
    }

    inline char* deserializeString()	{
        return (char*)deserializeNoCopy((int)strlen(&params[numOfParamsDeserialized])+1);    // +1: include the termnating 0
    }


    /***********************************************************/
    /***     SHORT CUTS : the same can be done by calling     **/
    /***     serialize and deserialize methods directly       **/
    /***********************************************************/


    inline char deserializeChar() 		{ char x;      deserialize(&x, sizeof(x)); return x; }
    inline short deserializeShort() 		{ short x;     deserialize(&x, sizeof(x)); return x; }
    inline int 	deserializeInt() 		{ int x;       deserialize(&x, sizeof(x)); return x; }
    inline long deserializeLong()		{ long x;      deserialize(&x, sizeof(x)); return x; }
    inline long long deserializeLonglong() 	{ long long x; deserialize(&x, sizeof(x)); return x; }
    inline float deserializeFloat() 		{ float x;     deserialize(&x, sizeof(x)); return x; }
    inline double deserializeDouble() 		{ double x;    deserialize(&x, sizeof(x)); return x; }

    /** to send, use for example
    	monitoring << counter1 << "ist working" << 3.141592 << EOM(MSG_ID);
    **/

    inline MonitoringMsg& operator<<(char a)      { serialize(a); return *this; }
    inline MonitoringMsg& operator<<(short a)     { serialize(a); return *this; }
    inline MonitoringMsg& operator<<(int a)       { serialize(a); return *this; }
    inline MonitoringMsg& operator<<(long a)      { serialize(a); return *this; }
    inline MonitoringMsg& operator<<(long long a) { serialize(a); return *this; }
    inline MonitoringMsg& operator<<(float a)     { serialize(a); return *this; }
    inline MonitoringMsg& operator<<(double a)    { serialize(a); return *this; }

    inline MonitoringMsg& operator<<(char* a)     { serializeString(a); return *this; }

    /// Only for End Of Message
    inline void  operator<<(MessageTypeId mtid)    { report(mtid.id); }

    /*********************************************/

    /** use for example
    	reportMessage >> int_counter >> string_array >> double_val;
    **/

    inline MonitoringMsg& operator>>(char &a)      { deserialize(&a); return *this; }
    inline MonitoringMsg& operator>>(short &a)     { deserialize(&a); return *this; }
    inline MonitoringMsg& operator>>(int &a)       { deserialize(&a); return *this; }
    inline MonitoringMsg& operator>>(long &a)      { deserialize(&a); return *this; }
    inline MonitoringMsg& operator>>(long long &a) { deserialize(&a); return *this; }
    inline MonitoringMsg& operator>>(float &a)     { deserialize(&a); return *this; }
    inline MonitoringMsg& operator>>(double &a)    { deserialize(&a); return *this; }

    // inline MonitoringMsg& operator>>(char* a)      { a = deserializeString(); return *this; } ERROR?: has no effect to *a ??

};


extern Topic<MonitoringMsg> monitorMsgTopic;
extern Topic<MonitoringMsg>  dbgCommandTopic;


#ifndef NO_RODOS_NAMESPACE
}
#endif

