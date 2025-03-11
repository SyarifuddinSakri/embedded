#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include "setup.h"
#include "blink.h"

int main(void) {
	clock_init();
	spi2_init();
	st7735_init();

	blink();


}
