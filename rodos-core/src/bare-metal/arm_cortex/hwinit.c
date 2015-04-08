#include <string.h>
#include "hwinit.h"
#include "hw/hw_types.h"
#include "hw/hw_memmap.h"
#include "hw/hw_ints.h"
#include "src/timer.h"
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
#include "utils/ustdlib.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "sd_fatfs/src/ff.h"
#include "sd_fatfs/src/diskio.h"
#include "utils/time.h"
#include "utils/flash_save.h"
#include "lwipopts.h"
extern unsigned long long nanoTime;
//*****************************************************************************
//
// Flags that contain the current value of the interrupt indicator
//
//*****************************************************************************
unsigned long g_ulFlags;
//*****************************************************************************
//
// Function declared in udp_lwip. It should be invoke in ethernet intrrupt
// function
//
//*****************************************************************************
extern void ethernetif_input(struct netif *netif);
//*****************************************************************************
//
// Function declared in udp_lwip. It need for configuration
// ethernet
//
//*****************************************************************************
extern err_t ethernetif_init(struct netif *netif);
//*****************************************************************************
//
// This variable declared in udp_srv.c is need for save ethernet configuration 
//
//*****************************************************************************
struct netif g_sEMAC_if;
	
//*****************************************************************************
//
// This variable declared in udp_srv.c.
// Create one instance of permanents variables storage in Flash 
// IP adresses and number of ports
//
//*****************************************************************************
PERMANENT_VAR permanentVars;

//*****************************************************************************
//
// The following is data structure used by FatFs.
//
//*****************************************************************************
static FATFS g_sFatFs;


//*****************************************************************************
//
// !Function initialize all hardware need for udp server work.
// !This function initialize UART if RS232 is declared.
// !If RS232 is declared this function write to console information
// ! and path to current directory
// ! \param none
// ! \return 0 when is OK and 1 when error occour
//
//*****************************************************************************
int hardwareInit()
{
    unsigned long ulUser0, ulUser1;
    unsigned char pucMACArray[8];
    char errEth=0;
    FRESULT fresult;
    unsigned long x,z;
	//
	    // Set the system clock to run at 8 MHz from the main oscillator.
	    //
//	    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |
//	                   SYSCTL_XTAL_8MHZ | SYSCTL_OSC_MAIN);

	    // Set the system clock to run at 50 MHz.
	    //
	    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL |
	                   SYSCTL_XTAL_8MHZ | SYSCTL_OSC_MAIN);
	    //
	    // Enable the peripherals used by this example.
	    //

	    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);	    
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	    //
	    // Configure the two 32-bit periodic timers.
	    //
	    TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	    //
	    // Configure Timer0 for a 100Hz interrupt.  The FatFs driver
	    // wants a 10 ms tick.
	    //
	    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 100);

	    //
	    // Setup the interrupts for the timer timeouts.
	    //
	    IntEnable(INT_TIMER0A);
	    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	    //
	    // Check for presence of Ethernet Controller.
	    //   
	    if(!SysCtlPeripheralPresent(SYSCTL_PERIPH_ETH))
	    {
	    	errEth=1;
	    }
	    //
	    // Enable and Reset the Ethernet Controller.
	    //
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
	    SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);
	    EthernetIntDisable(ETH_BASE, ETH_INT_TX);
	    //
	    // Enable Port F for Ethernet LEDs.
	    //  LED0        Bit 3   Output
	    //  LED1        Bit 2   Output
	    //
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_DIR_MODE_HW);
	    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3,
	                     GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);


	    //
	    // Configure SysTick for a 100Hz interrupt.  The FatFs driver
	    // wants a 10 ms tick.
	    //
	    SysTickPeriodSet(SysCtlClockGet() / 100);
//	    SysTickEnable();
//	    SysTickIntEnable();
	    //
	    // Enable the Hibernation module.
	    //
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
	    HibernateEnableExpClk(SysCtlClockGet());
	    //
	    // Wait an amount of time for the module to power up.
	    //
	    for(x=0;x<10000;++x)
	    	for(z=0;z<10000;++z);
	    //
	    // Configure the clock source for Hibernation module, and enable the
	    // RTC feature. This configuration is for the 4.194304 MHz crystal.
	    //
	    HibernateClockSelect(HIBERNATE_CLOCK_SEL_DIV128);
	    HibernateRTCEnable();

	    //
	    // Enable Interrupts
	    //
	    IntMasterEnable();
	    //
	    // Set GPIO A0 and A1 as UART.
	    //
	    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	    //
	    // Initialize the UART as a console for text I/O.
	    //
	    UARTStdioInit(0);
	    //
	    // Enable the UART interrupt.
	    //
	    //IntEnable(INT_UART0);
	    //UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); 
	    if(errEth)
	    {
	    	UARTprintf("\n\nEthernet Controller - Not Found!\n");
	    }    
	    //
	    // Configure the hardware MAC address for Ethernet Controller filtering of
	    // incoming packets.
	    //
	    // For the LM3S6965 Evaluation Kit, the MAC address will be stored in the
	    // non-volatile USER0 and USER1 registers.  These registers can be read
	    // using the FlashUserGet function, as illustrated below.
	    //
	    FlashUserGet(&ulUser0, &ulUser1);
	    if((ulUser0 == 0xffffffff) || (ulUser1 == 0xffffffff))
	    {
	            //
	            // We should never get here.  This is an error if the MAC address
	            // has not been programmed into the device.  Exit the program.
	            //
#ifdef RS232
        		UARTprintf("\n\nMAC Address - Not Programmed!\n");
#endif
	        	errEth=1;
	    }

	    //
	    // Convert the 24/24 split MAC address from NV ram into a 32/16 split
	    // MAC address needed to program the hardware registers, then program
	    // the MAC address into the Ethernet Controller registers.
	    //
	    pucMACArray[0] = ((ulUser0 >>  0) & 0xff);
	    pucMACArray[1] = ((ulUser0 >>  8) & 0xff);
	    pucMACArray[2] = ((ulUser0 >> 16) & 0xff);
	    pucMACArray[3] = ((ulUser1 >>  0) & 0xff);
	    pucMACArray[4] = ((ulUser1 >>  8) & 0xff);
	    pucMACArray[5] = ((ulUser1 >> 16) & 0xff);

	    //
	    // Program the hardware with it's MAC address (for filtering).
	    //
	    EthernetMACAddrSet(ETH_BASE, pucMACArray);
	    
	    //
	    // Mount the file system, using logical disk 0.
	    //
		fresult = f_mount(0, &g_sFatFs);
		if(fresult != FR_OK)
		{
#ifdef RS232
//			UARTprintf("f_mount error: %s\n", StringFromFresult(fresult));
#endif
//			return(1);
		}
	    
		if(errEth)
		{
#ifdef RS232
			UARTprintf("Initiate Ethernet card error\n");
#endif
	//		return(1);
		}
	    HibernateRTCSet(date2day(2010,7,29,12,45,0));
	    
	    //
	    // Enable the timer0.
	    //
	    TimerEnable(TIMER0_BASE, TIMER_A);

	    UARTprintf("Hardware init\n");
	    return(0);
}
//*****************************************************************************
//
// !Function should be called by the top-level application to perform the needed 
// ! lwIP TCP/IP initialization
// ! \param none
// ! \return none
//
//*****************************************************************************
void
lwip_init(void)
{
#ifdef LWIP_DEBUG  
	unsigned long ipaddr;
	char pucBuf[16];
	int iIndex = 0;
	int iTemp;
	unsigned char *pucTemp;
#endif

    //
    // Low-Level initialization of the lwIP stack modules.
    //
    stats_init();
    sys_init();
    mem_init();
    memp_init();
    pbuf_init();
    netif_init();
    etharp_init();
    ip_init();
    udp_init();
   

    //
    // Create, Configure and Add the Ethernet Controller Interface.
    //
    
    // read ethernet configuration from flash
    readPermanentsFromFlash(&permanentVars);
    
    //*******************************************************
    // if the IP-Parameter should be loaded from flash 
    // the source under this comment should be makrked as comment
    // otherwise the IP parameter below are pending
    //*******************************************************
    if(permanentVars.progFlag != 0xAA55)
    {
    	IP4_ADDR(&(permanentVars.xIpAddr),192,168,0,20);
    	IP4_ADDR(&(permanentVars.xNetMask),255,255,0,0);
    	IP4_ADDR(&(permanentVars.xGateway),192,168,0,1);
    	permanentVars.portListen=995;
    	permanentVars.portSend=996;
    	permanentVars.progFlag = 0xAA55;
    	writePermanentsToFlash(permanentVars);
    	  	
    }
  
    //*******************************************************
    // end of IP config
    //*******************************************************

    netif_add(&g_sEMAC_if,
              &(permanentVars.xIpAddr),
              &(permanentVars.xNetMask),
              &(permanentVars.xGateway),
              NULL,
              ethernetif_init,
              ip_input);
    netif_set_default(&g_sEMAC_if);

    //
    // Bring the interface up.
    //
    netif_set_up(&g_sEMAC_if);
#ifdef LWIP_DEBUG   
    ipaddr=g_sEMAC_if.ip_addr.addr;
    pucTemp = (unsigned char *)&ipaddr;
    iTemp = usprintf(&pucBuf[iIndex], "%d.", pucTemp[0]);
    iIndex += iTemp;
    iTemp = usprintf(&pucBuf[iIndex], "%d.", pucTemp[1]);
    iIndex += iTemp;
    iTemp = usprintf(&pucBuf[iIndex], "%d.", pucTemp[2]);
    iIndex += iTemp;
    iTemp = usprintf(&pucBuf[iIndex], "%d", pucTemp[3]);
    iIndex += iTemp;
    pucBuf[iIndex] = 0;
    
    UARTprintf("IP addres: %s\n", pucBuf);
#endif 

    return;
}
//*****************************************************************************
//
// !Ethernet Interrupt handler.
// ! \param none
// ! \return none
//
//*****************************************************************************
void
EthernetIntHandler(void)
{
    unsigned long ulTemp;
    
//    UARTprintf("Ethernet INT\n");
    //
    // Read and Clear the interrupt.
    //
    ulTemp = EthernetIntStatus(ETH_BASE, false);
    EthernetIntClear(ETH_BASE, ulTemp);
    
    ethernetif_input(&g_sEMAC_if); 
}
//*****************************************************************************
//
// The interrupt handler for the first timer interrupt. 
// !FatFs requires a timer tick every 10 ms for internal timing purposes.
// ! \param none
// ! \return none
//
//
//*****************************************************************************
void
Timer0IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //
    // Toggle the flag for the first timer.
    //
    HWREGBITW(&g_ulFlags, 0) ^= 1;

    //
    // Call the FatFs tick timer.
    //
    disk_timerproc();

   nanoTime +=(10000LL*1000LL);
}

