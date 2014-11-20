#include <stdio.h>
#include <stdlib.h>
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

    PinCfg.Funcnum = 1;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg.Pinnum = 24;
    PinCfg.Portnum = 0;
    PINSEL_ConfigPin(&PinCfg);

    ADC_Init(LPC_ADC, 10000);
    ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, DISABLE);
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_1, ENABLE);
}

static int dblcmp (const void * a, const void * b) {
  if (*(double*)a > *(double*)b) return 1;
  else if (*(double*)a < *(double*)b) return -1;
  else return 0;
}

double mode(double nums[], int size) {
    qsort(nums, size, sizeof(double), dblcmp);
    int count = 1;
    int max_count = count;
    double prev = nums[0];
    double n = prev;
    int i;

    for (i = 1; i < size; i++) {
        if (nums[i] == prev) {
            count++;
        } else {
            count = 1;
        }
        if (count > max_count) {
            n = prev;
            max_count = count;
        }

        prev = nums[i];
    }

    return n;
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
    char message[7];
    int mode_n = 10;
    double values[mode_n];
    int n = 0;
    double last_value = 0;
    double current_value;

    while(1) {
        // Read analogue value
        ADC_StartCmd(LPC_ADC,ADC_START_NOW);
        // Wait conversion complete
        while (!(ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_1,ADC_DATA_DONE)));

        adc_value = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_1);
        values[n] = (double) adc_value;

        if (n < mode_n) {
            n++;
        } else {
            current_value = mode(values, sizeof(values)/sizeof(double)) / 1240.0;
            if (current_value != last_value) {
                sprintf(message, "%1.2fV\n\r", current_value);
                write_usb_serial_blocking(message, sizeof(message));
            }
            last_value = current_value;
            n = 0;
        }

    }

    return 0;
}
