#include "lpc17xx_gpio.h"
#include "LPC17xx.h"

int duration_passed = 0;

void SysTick_Handler(void) {
    duration_passed = 1;
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

int main(void) {
    int x;
    int delay;
    delay = 1 << 21;

    GPIO_SetDir(1, (101101 << 18), 1);

    int i;

    while (1) {
        for (i = 0; i < 16; i++) {
            display_num(i);
            while (duration_passed == 0);
        }
    }

    return 0;
}
