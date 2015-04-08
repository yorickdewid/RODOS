#include "hw_specific.h"
#include "rodos.h"
#include "scheduler.h"
#include "File.h"
#include <string.h>

class E : public Thread {
  private:
	    long cnt;
	    
  public:
	    E() : Thread("MyThread E",5, 1024) {
	    	cnt=0;
	    }
	    void run() {
	    	
	    	xprintf("I am thread E.....1 \r\n");
	    	TIME_LOOP(250*MILLISECONDS,2*SECONDS) {
	    	 ++cnt;
	         xprintf("I am thread E^ cnt = %ld\r\n",cnt);
	       }
	    }
}e;

class F : public Thread {
  private:
	    long cnt;
	    
  public:
	    F() : Thread("MyThread F",6, 1024) {
	    	cnt=0;
	    }
	    void run() {
	    	
	    	xprintf("I am thread F.....1 \r\n");
	    	TIME_LOOP(250*MILLISECONDS,2*SECONDS) {
	    	 ++cnt;
	         xprintf("I am thread F^ cnt = %ld\r\n",cnt);
	       }
	    }
}f;


