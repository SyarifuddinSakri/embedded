#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include "blink.h"

void blink(void){
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    gpio_toggle(GPIOC, GPIO13);
}
void delay(uint32_t period){
    for (uint32_t i=0; i<period; i++){
    __asm__("nop");
    }
}
