#include <libopencm3/stm32/gpio.h>
#include "blink.h"

void blink(void){
        gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
        while(1){
                for (int i=0; i<10000000; i++){
                __asm__("nop");
                }
        gpio_toggle(GPIOC, GPIO13);
        }
}
