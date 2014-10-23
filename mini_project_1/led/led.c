#include "lpc17xx_gpio.h"
#include "LPC17xx.h"

int duration_passed = 0;

void SysTick_Handler(void) {
        duration_passed++;
}

int main (void) {
    int i;
    int n;
    int leds[] = {18, 20, 21, 23};

    SysTick_Config(SystemCoreClock / 6);

    GPIO_SetDir(1, (101101 << 18), 1);

    for (n = 0; n < 5; n++) {
        for (i = 0; i < 4; i++) {
            GPIO_SetValue(1, (1 << leds[i]));

            while (duration_passed != 5);
            duration_passed = 0;

            GPIO_ClearValue(1, (1 << leds[i]));
        }
    }

    return 0;
}
