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

void get_bin(int num, char *str) {
    *(str+5) = '\0';
    int mask = 0x8 << 1;
    while(mask >>= 1)
    *str++ = !!(mask & num) + '0';
}

void display_num(int n) {
    int leds[] = {0, 18, 20, 21, 23};
    char bin_string[4];
    int i;

    get_bin(n, bin_string);

    GPIO_ClearValue(1, (101101 << 18));

    for (i = 0; i < 4; i++) {
        if (bin_string[i] == '1') {
            GPIO_SetValue(1, (1 << leds[i + 1]));
        }
    }
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

void i2c_write_bytes(I2C_M_SETUP_Type * i2c_config, int address, uint8_t bytes[], int length) {
    i2c_config->tx_data = bytes;
    i2c_config->tx_length = length;
    i2c_config->sl_addr7bit = address;
    I2C_MasterTransferData(LPC_I2C1, i2c_config, I2C_TRANSFER_POLLING);
}

void lcd_write_message(I2C_M_SETUP_Type * i2c_config, char message[], int length) {
    uint8_t data_write[58] = { [0 ... 57] = 0xA0};
    int i;
    int p;
    for (i = 0; i < sizeof(data_write); i++) {
        if (i == 0) {
            data_write[i] = 0x40;
        } else {

            if (i < length) {
                if (i > 16) {
                    p = i + 24;
                } else {
                    p = i;
                }

                if (message[i-1] < 91) {
                    data_write[p] = message[i-1] + 128;
                } else if (message[i-1] == ' ') {
                    data_write[p] = 0xA0;
                } else {
                    data_write[p] = message[i-1];
                }
            }

        }
    }

    i2c_write_bytes(i2c_config, 59, data_write, 58);
}

int main(void) {
    init_i2c();

    SysTick_Config(SystemCoreClock / 6);
    GPIO_SetDir(1, (101101 << 18), 1);

    uint8_t response = 5;

	I2C_M_SETUP_Type I2CConfigStruct;
    I2CConfigStruct.retransmissions_max = 3;
    I2CConfigStruct.rx_data = &response;
    I2CConfigStruct.rx_length = 1;

    uint8_t bytes[] = {0x0F};
    i2c_write_bytes(&I2CConfigStruct, 33, bytes, sizeof(bytes));

    while (1) {
        i2c_write_bytes(&I2CConfigStruct, 33, bytes, sizeof(bytes));
        display_num(response);
    }

    return 0;
}
