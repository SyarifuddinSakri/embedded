#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include "print.h"

int main(void){
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	uart1_setup();
	//Enable GPIOC
  rcc_periph_clock_enable(RCC_GPIOC);
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	uint64_t time = 0x12345678ABCDEF01ULL;
	uint32_t time_high = time>>32;
	uint32_t time_low = time;
	int buntut = 190;
	printf("Makan nasi la\n");
	printf("value of bontot is %d\n",buntut);
	printf("Value high os %u\n", time_high);
	printf("Value low os %u\n", time_low);
	printf("High = 0x%08X, Low = 0x%08X\n", time_high, time_low);
	printf("Full 64-bit value = 0x%08lX%08lX\n", (unsigned long)time_high, (unsigned long)time_low);

	for(;;){

	}
}

