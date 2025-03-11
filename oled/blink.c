#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include "blink.h"

void blink(void){
    delay(1000000);
    gpio_toggle(GPIOC, GPIO13);
}
void delay(uint32_t period){
    for (uint32_t i=0; i<period; i++){
    __asm__("nop");
    }
}
