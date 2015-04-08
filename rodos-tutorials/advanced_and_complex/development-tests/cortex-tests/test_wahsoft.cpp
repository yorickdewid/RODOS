#include "hw_specific.h"
#include "rodos.h"
#include "scheduler.h"
#include "File.h"
#include <string.h>

static File root;
static File file1("/file.txt");
static UDPIn udpIn(995);
static UDPOut udpOut(996,16820416);// 16820416 it is addr=192.168.0.1

void MAIN() {
	setMyIP(192,168,0,40);
	setMyIPMask(255,255,255,0);
	xprintf("\nSet IP: 192.168.0.40 \n");
	root.open();
}


class A : public Thread {
  private:
	    long cnt;
	    char  userData[1000];
	    long total;
	    
  public:
	    A() : Thread("MyThread A",2, 1024) {
	    	cnt=0;
	    }
	    void run() {
	    	
	    	xprintf("I am thread A.....1 \r\n");
	    	root.ls(userData,1000);
	    	xprintf("\n ls: \n %s\n",userData);
	    	
	    	total=totalSpace();
	        xprintf("\n TotalSpace = %d\n",total);
	        
	        total=freeSpace();
	        xprintf("\n FreeSpace = %dkB\n",total);
	    	TIME_LOOP(250*MILLISECONDS,3*SECONDS) {
	    	 ++cnt;
	         xprintf("\r\n-->I am thread A^ cnt = %ld\r\n",cnt);
	       }
	    }
}a;

class B : public Thread {
  private:
	  char  userData[1000];
	  unsigned long ipAddr;
		int z;
		bool ok;
		long total;
	    long cnt;
	    
  public:
	    B() : Thread("MyThread B",1, 2048) {
	    	cnt=0;
	    }
	    void run() {
	    	z=0;
	    	
	    	if(udpIn.isConnected()==false)
	    	{
	    		xprintf("\n UDP reopen\n");
	    		udpIn.reopen(995);
	    	}
	    	
	    	xprintf("\r\n--->I am thread B.....1 \r\n");
	    	file1.open();
	    	if(file1.errorNo == FR_OK && file1.fileType==PLAIN_FILE)
	    	{
	    		xprintf("\nFile /file.txt exist\n");
	    		file1.close();
	    	}
	    	else
	    	{
	    		file1.create();
	    		char *wBuf="\nTest write to file\n";
	    		file1.write(wBuf,21);
	    		file1.close();
	    	}
	    	TIME_LOOP(350*MILLISECONDS,1*SECONDS) {
	    		++cnt;
	    		xprintf("I am thread B* cnt = %ld\r\n",cnt);
	    		ok = udpIn.readyToGet();
	    		if(ok)
	    		{
	        	    int i,dataIndex;
	        	    char cmd[20];
	        	    
	    			z=udpIn.get(userData,1000,&ipAddr);
	    			xprintf("\nRcv UDP len=%d data=%s",z,userData);
	    			xprintf("\nFrom ip addr = %ld\n", ipAddr);
	    			
	    	  		dataIndex=strlen(userData);
	    	  		++dataIndex;
	            	i=strcspn(userData," ");
	    	  		strcpy(cmd,userData);
	    	  		cmd[i]=0x00;
	    	  		
	    	  		if(strcmp(cmd,"get")==0)
	    	  		{
	    	  			if(file1.open())
	    	  			{
	    	  				int len = file1.read(&userData[dataIndex],900);
	    	  				file1.close();
	    	  				len+=dataIndex;
	    	  				xprintf("\nSend = %s\n",userData);
	    	  				udpOut.sendTo(userData,len,ipAddr); // recive addr
	    	  				file1.close();
	    	  			}
	    	  			else
	    	  			{
	    		  			char *data="\nFile no exist \n";
	    		  			int len = strlen(data);
	    		  			strncpy(&userData[dataIndex],data,len);
	    		  			len+=dataIndex;
	    		  			xprintf("\nSend = %s\n",userData);
	    		  			udpOut.sendTo(userData,len,ipAddr); // recive addr
	    	  			}
	    	  		}
	    	  		else if(strcmp(cmd,"ls")==0)
	    	  		{
	    	  			int len = root.ls(&userData[dataIndex],900);
	    	  			len+=dataIndex;
	    	  			xprintf("\nSend = %s\n",userData);
	    	  			udpOut.sendTo(userData,len,ipAddr); // recive addr
	    	  		}
	    	  		else if(strcmp(cmd,"del")==0)
	    	  		{
	    	  		    rm("/file.txt");
	    	  		    rmdir("/test");
	    	  			char *data="\nDeleted !!! \n";
	    	  			int len = strlen(data);
	    	  			strncpy(&userData[dataIndex],data,len);
	    	  			len+=dataIndex;
	    	  			xprintf("\nSend = %s\n",userData);
	    	  			udpOut.sendTo(userData,len,ipAddr); // recive addr
	    	  		}
	    	  		else
	    	  		{
	    	  			char *data="\nUnknown command \n";
	    	  			int len = strlen(data);
	    	  			strncpy(&userData[dataIndex],data,len);
	    	  			len+=dataIndex;
	    	  			xprintf("\nSend = %s\n",userData);
	    	  			udpOut.sendTo(userData,len,ipAddr); // recive addr
	    	  		}
	    		}
	       }
	    }
}b;

