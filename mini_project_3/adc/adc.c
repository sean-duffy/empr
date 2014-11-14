#include <stdio.h>
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_adc.h"
#include "LPC17xx.h"

int duration_passed = 0;

void SysTick_Handler(void) {
    duration_passed++;
}

void init_adc(void) {
    PINSEL_CFG_Type PinCfg; 

    PinCfg.Funcnum = 3;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Pinnum = 30;
    PinCfg.Portnum = 1;
    PINSEL_ConfigPin(&PinCfg);

    ADC_Init(LPC_ADC, 10000);
    ADC_IntConfig(LPC_ADC, ADC_ADINTEN4, DISABLE);
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_4, ENABLE);
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

int main(void) {
    serial_init();
    init_adc();
    SysTick_Config(SystemCoreClock / 6);
    
    uint16_t adc_value;
    char message[30];
    int i;

    write_usb_serial_blocking("Hati\n\r",5);

    while(1) {
        // read value of potencjometer
        ADC_StartCmd(LPC_ADC,ADC_START_NOW);
        //Wait conversion complete
        while (!(ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_4,ADC_DATA_DONE)));

        adc_value = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_4);
        sprintf(message, "ChannelGetData Data: %05d\n\r", adc_value);
        write_usb_serial_blocking(message, sizeof(message));
    }


    return 0;
}
