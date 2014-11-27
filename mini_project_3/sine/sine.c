#include <stdio.h>
#include <math.h>

#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_dac.h"
#include "LPC17xx.h"

int current_tick = 0;
int sine_buff[360];
int duration_passed = 0;

void SysTick_Handler(void) {
    if (current_tick >= 360) {
        current_tick = 0;
    }

    DAC_UpdateValue(LPC_DAC, sine_buff[current_tick]);
    current_tick += 5;
    duration_passed++;
}

void init_dac(void) {
    PINSEL_CFG_Type PinCfg; 

    PinCfg.Funcnum = 2;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    
    //change pin number
    PinCfg.Pinnum = 26;
    PinCfg.Portnum = 0;
    PINSEL_ConfigPin(&PinCfg);

    DAC_Init (LPC_DAC);
}

// Write options
int write_usb_serial_blocking(char *buf,int length)
{
	return(UART_Send((LPC_UART_TypeDef *)LPC_UART0,(uint8_t *)buf,length, BLOCKING));
}

// init code for the USB serial line
void serial_init(void)
{
	UART_CFG_Type UARTConfigStruct;			// UART Configuration structure variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;	// UART FIFO configuration Struct variable
	PINSEL_CFG_Type PinCfg;				// Pin configuration for UART
	/*
	 * Initialize UART pin connect
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	// USB serial first
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);
		
	/* Initialize UART Configuration parameter structure to default state:
	 * - Baudrate = 9600bps
	 * - 8 data bit
	 * - 1 Stop bit
	 * - None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	/* Initialize FIFOConfigStruct to default state:
	 * - FIFO_DMAMode = DISABLE
	 * - FIFO_Level = UART_FIFO_TRGLEV0
	 * - FIFO_ResetRxBuf = ENABLE
	 * - FIFO_ResetTxBuf = ENABLE
	 * - FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	// Built the basic structures, lets start the devices/
	// USB serial
	UART_Init((LPC_UART_TypeDef *)LPC_UART0, &UARTConfigStruct);		// Initialize UART0 peripheral with given to corresponding parameter
	UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART0, &UARTFIFOConfigStruct);	// Initialize FIFO for UART0 peripheral
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART0, ENABLE);			// Enable UART Transmit
}

void wave(double freq, double amplitude) {
    freq = 1/freq * 1389000;
    amplitude = amplitude * 309.1;
    int res = 360;

    int i;
    for(i = 0; i < 360; i++) {
        sine_buff[i] = (int) floor((amplitude * sin((2*M_PI/res)*i) + amplitude) / 2);
    }

    write_usb_serial_blocking("Go!\n\r", 5);

    SysTick_Config((int) floor(freq));
}
int main(void) {
    serial_init();
    init_dac();

    while(1) {
        wave(4000, 2.5);
        while (duration_passed != 1000000);
        duration_passed = 0;

        wave(7000, 3.3);
        while (duration_passed != 1000000);
        duration_passed = 0;

        wave(6000, 1.5);
        while (duration_passed != 1000000);
        duration_passed = 0;
    }

    return 0;
}
