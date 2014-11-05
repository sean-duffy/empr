#include <string.h>
#include <stdio.h>
#include "lpc17xx_i2c.h"
#include "lpc_types.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "LPC17xx.h"

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

void init_i2c(void) {
 PINSEL_CFG_Type PinCfg; // declare data struct with param members
 PinCfg.OpenDrain = 0;
 PinCfg.Pinmode = 0;
 PinCfg.Funcnum = 3;
 PinCfg.Pinnum = 0;
 PinCfg.Portnum = 0;
 PINSEL_ConfigPin(&PinCfg); // configure pin 0 of port0
 PinCfg.Pinnum = 1;
 PINSEL_ConfigPin(&PinCfg); // configure pin 1 of port0
 I2C_Init(LPC_I2C1, 100000); // Initialize I2C peripheral
 I2C_Cmd(LPC_I2C1, ENABLE); // Enable I2C1 operation 
}

void I2C_write(int addr,int data_8bit){
    
    Status result;	
    I2C_M_SETUP_Type I2CConfigStruct;
    I2CConfigStruct.tx_data = data_8bit; 
    I2CConfigStruct.tx_length = 8;
    I2CConfigStruct.retransmissions_max = 3;
    I2CConfigStruct.rx_length = 0;
    I2CConfigStruct.rx_data = 0;

    I2CConfigStruct.sl_addr7bit = addr;

    char str[50];
    sprintf(str,"Writing (%d) to address (%b)\t", data_8bit,addr);
    write_usb_serial_blocking(str,50);

    result = I2C_MasterTransferData(LPC_I2C1, &I2CConfigStruct, I2C_TRANSFER_POLLING);
    
    if (result == SUCCESS){
        write_usb_serial_blocking("Success\n\r",10);
    } else {
        write_usb_serial_blocking("Failure\n\r",10);
    }
       
}

void I2C_read(int addr){

    Status result;
    I2C_M_SETUP_Type I2CConfigStruct; 
    I2CConfigStruct.tx_length = 0;
    I2CConfigStruct.retransmissions_max = 3;
    I2CConfigStruct.rx_length = 8;

    I2CConfigStruct.sl_addr7bit = addr;

    char str[50];
    sprintf(str,"Reading () from address (data_8bit)\t", addr);
    write_usb_serial_blocking(str,50);

    result = I2C_MasterTransferData(LPC_I2C1, &I2CConfigStruct, I2C_TRANSFER_POLLING);
    
    if (result == SUCCESS){
        write_usb_serial_blocking("Success\n\r",10);
    } else {
        write_usb_serial_blocking("Failure\n\r",10);
    }
}
int main(void) {
    serial_init();
    init_i2c();

    I2C_write(32,1111);

    Status result;	
    I2C_M_SETUP_Type I2CConfigStruct;
    I2CConfigStruct.tx_data = 128; // 1000 0000
    I2CConfigStruct.tx_length = 8;
    I2CConfigStruct.retransmissions_max = 3;
    I2CConfigStruct.rx_length = 8;
    I2CConfigStruct.rx_data = 0;

    I2CConfigStruct.sl_addr7bit = 33;
    
    write_usb_serial_blocking("Trying address 33\n\r", 19);
    
    result = I2C_MasterTransferData(LPC_I2C1, &I2CConfigStruct, I2C_TRANSFER_POLLING);
    if (result == SUCCESS){
        write_usb_serial_blocking("Success\n\r",10);
    } else {
        write_usb_serial_blocking("Failure\n\r",8);
    }

    int i;
    for (i = 0; i< 256; i++){
        I2CConfigStruct.tx_data = i;
        I2CConfigStruct.tx_length = 8;
        result = I2C_MasterTransferData(LPC_I2C1, &I2CConfigStruct, I2C_TRANSFER_POLLING);
        
        I2CConfigStruct.tx_data = 0;
        I2CConfigStruct.tx_length = 0;
        result = I2C_MasterTransferData(LPC_I2C1, &I2CConfigStruct, I2C_TRANSFER_POLLING);
        
        char str[50];
        sprintf(str,"return data: %d\t",I2CConfigStruct.rx_data);
        write_usb_serial_blocking(str,20);
    }
    

    write_usb_serial_blocking("\nDone\n\r",10);

    /*int count = 0;
    int i;
    char addr_str[50];
    for (i = 0; i < ( 0xFF >> 1); i++) {
        I2CConfigStruct.sl_addr7bit = i;
        result = I2C_MasterTransferData(LPC_I2C1, &I2CConfigStruct, I2C_TRANSFER_POLLING);
        if (result == SUCCESS) {
            sprintf(addr_str, "device found at address %d\r\n", i);
            write_usb_serial_blocking(addr_str, strlen(addr_str));
            count++;
        }
    }

    char count_str[50];
    sprintf(count_str, "%d devices connected to i2c bus\r\n", count);
    write_usb_serial_blocking(count_str, strlen(count_str));
    */

    return 0;
}
