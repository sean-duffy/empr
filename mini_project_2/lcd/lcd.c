#include <string.h>
#include <stdio.h>
#include "lpc17xx_gpio.h"
#include "lpc17xx_i2c.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "LPC17xx.h"

int duration_passed = 0;

void SysTick_Handler(void) {
    duration_passed++;
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

int main(void) {
    Status result;
    init_i2c();

    SysTick_Config(SystemCoreClock / 6);
    GPIO_SetDir(1, (101101 << 18), 1);

	I2C_M_SETUP_Type I2CConfigStruct;
    I2CConfigStruct.retransmissions_max = 3;
    I2CConfigStruct.sl_addr7bit = 59;

    uint8_t lcd_init[6] = {0x00, 0x35, 0x9F, 0x34, 0x0C, 0x02};
    I2CConfigStruct.tx_data = lcd_init;
    I2CConfigStruct.tx_length = 6;
    result = I2C_MasterTransferData(LPC_I2C1, &I2CConfigStruct, I2C_TRANSFER_POLLING);

    uint8_t clear[2] = {0x00, 0x01};
    I2CConfigStruct.tx_data = clear;
    I2CConfigStruct.tx_length = 2;
    result = I2C_MasterTransferData(LPC_I2C1, &I2CConfigStruct, I2C_TRANSFER_POLLING);

    while (duration_passed != 10);
    duration_passed = 0;

    uint8_t lcd_write[3] = {0x00, 0x40, 0x50};
    I2CConfigStruct.tx_data = lcd_write;
    I2CConfigStruct.tx_length = 3;
    result = I2C_MasterTransferData(LPC_I2C1, &I2CConfigStruct, I2C_TRANSFER_POLLING);

    GPIO_SetValue(1, (1 << 18));

    return 0;
}
