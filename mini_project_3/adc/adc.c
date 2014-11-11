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
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg.Funcnum = 3;
    PinCfg.Pinnum = 30;
    PinCfg.Portnum = 1;
    PINSEL_ConfigPin(&PinCfg);

    ADC_Init(LPC_ADC, 10000);
    ADC_StartCmd(LPC_ADC, ADC_START_NOW);
    ADC_BurstCmd(LPC_ADC, 1);
    ADC_ChannelCmd(LPC_ADC, 0, ENABLE);
    //ADC_ChannelCmd(LPC_ADC, 1, ENABLE);
    //ADC_ChannelCmd(LPC_ADC, 2, ENABLE);
    //ADC_ChannelCmd(LPC_ADC, 3, ENABLE);
    //ADC_ChannelCmd(LPC_ADC, 4, ENABLE);
    //ADC_ChannelCmd(LPC_ADC, 5, ENABLE);
    //ADC_ChannelCmd(LPC_ADC, 6, ENABLE);
    //ADC_ChannelCmd(LPC_ADC, 7, ENABLE);

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
    char message[31];
    int i;

    while (1) {
        for (i = 0; i < 8; i++) {
            adc_value = ADC_ChannelGetData(LPC_ADC, i);
            if (adc_value != 0){
                sprintf(message, "%dChannelGetData Data: %05d\n\r", i, adc_value);
                write_usb_serial_blocking(message, sizeof(message));
            }

            adc_value = ADC_GetData(i);
            if (adc_value != 0){
                sprintf(message, "%d       GetData Data: %05d\n\r", i, adc_value);
                write_usb_serial_blocking(message, sizeof(message));
            }

            //adc_value = ADC_GlobalGetData(LPC_ADC);
            //if (adc_value != 0){
            //    sprintf(message, "%d Global GetData Data: %05d\n\r", i, adc_value);
            //    write_usb_serial_blocking(message, sizeof(message));
            //}
        }
    }

    return 0;
}
