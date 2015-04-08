#include "uart/uart.h"
#include "registers/regsuart.h"
#include "core/ccm_pll.h"
#include "iomux_config.h"

#define UART_UFCR_RFDIV    BF_UART_UFCR_RFDIV(4)

uint32_t uart_get_reffreq(uint32_t instance)
{
    uint32_t div = UART_UFCR_RFDIV;
    uint32_t ret = 0;
    uint32_t freq = get_peri_clock(HW_UART2);

    if (div == BF_UART_UFCR_RFDIV(4))
        ret = freq / 2;
    else if (div == BF_UART_UFCR_RFDIV(2))
        ret = freq / 4;
    else if (div == BF_UART_UFCR_RFDIV(6))
        ret = freq / 7;

    return ret;
}


void UART_init(int instance){
	uint32_t base = REGS_UART_BASE(instance);
	//int instance = HW_UART2;
	int baudrate = 115200;

	/* configure the I/O for the port */
	uart_iomux_config(instance);

	/* enable the source clocks to the UART port */
	clock_gating_config(base, CLOCK_ON);

	/* Wait for UART to finish transmitting before changing the configuration */
	while (!(HW_UART_UTS(instance).B.TXEMPTY)) ;

	/* Disable UART */
	HW_UART_UCR1_CLR(instance, BM_UART_UCR1_UARTEN);

	/* Configure FIFOs trigger level to half-full and half-empty */
	HW_UART_UFCR_WR(instance, BF_UART_UFCR_RXTL(16) | UART_UFCR_RFDIV | BF_UART_UFCR_TXTL(16));

	/* Setup One Millisecond timer */
	//HW_UART_ONEMS_WR(instance, uart_get_reffreq(instance) / 1000);

	/* Set parity = none */
	HW_UART_UCR2_CLR(instance,(BM_UART_UCR2_PREN| BM_UART_UCR2_PROE));

	/* Set stop bit = 1 bit */
	HW_UART_UCR2_CLR(instance, BM_UART_UCR2_STPB);

	/* Set data size = 8 bits */
	HW_UART_UCR2_SET(instance, BM_UART_UCR2_WS);

	/* Configure the flow control = off */
	/* Ignore RTS */
	HW_UART_UCR2_SET(instance,  BM_UART_UCR2_IRTS);
	/* CTS controlled by the CTS bit */
	HW_UART_UCR2_CLR(instance,  BM_UART_UCR2_CTSC);

	/* the reference manual says that this bit must always be set */
	HW_UART_UCR3_SET(instance,  BM_UART_UCR3_RXDMUXSEL);

	/* Enable UART */
	HW_UART_UCR1_SET(instance, BM_UART_UCR1_UARTEN);

	/* Enable FIFOs and does software reset to clear status flags, reset
	       the transmit and receive state machine, and reset the FIFOs */
	HW_UART_UCR2_SET(instance, BM_UART_UCR2_TXEN | BM_UART_UCR2_RXEN | BM_UART_UCR2_SRST);

	/* Set the numerator value minus one of the BRM ratio */
	HW_UART_UBIR_WR(instance, (baudrate / 100) - 1);

	/* Set the denominator value minus one of the BRM ratio */
	HW_UART_UBMR_WR(instance,  ((uart_get_reffreq(instance) / 1600) - 1));

	/* Optional: prevent the UART to enter debug state. Useful when debugging
	       the code with a JTAG and without active IRQ */
	HW_UART_UTS_SET(instance, BM_UART_UTS_DBGEN);
}


void UART_writeChar(int instance, char c){
	while (HW_UART_UTS(instance).B.TXFULL);
	HW_UART_UTXD_WR(instance, c);
}

void UART_writeStr(int instance, const char *c){
	while (*c != 0){
		UART_writeChar(instance, *c);
		if(*c == '\n'){
			UART_writeChar(instance, '\r');
		}
		c++;
	}
}

char UART_readChar(int instance){
	unsigned int read_data;

	/* If Rx FIFO has no data ready */
	if (!(HW_UART_USR2(instance).B.RDR))
		return -1;

	read_data = HW_UART_URXD_RD(instance);

	/* If error are detected */
	if (read_data & 0x7C00)
		return -1;

	return (read_data & 0xFF);
}

