#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include "FreeRTOS.h"
#include "libopencm3/stm32/f1/gpio.h"
#include "projdefs.h"
#include "task.h"

void task1(void* args __attribute((unused)));
void task2(void* args __attribute((unused)));

int main(void) {
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13 | GPIO14);

	xTaskCreate(task1, "LED1", 128, NULL, 1, NULL);
	xTaskCreate(task2, "LED2", 128, NULL, 1, NULL);

	vTaskStartScheduler();

}

void task1(void* args __attribute((unused))){

	for(;;){
		gpio_toggle(GPIOC, GPIO13);
		vTaskDelay(pdMS_TO_TICKS(100));
	}

}

void task2(void* args __attribute((unused))){

	for(;;){
		gpio_toggle(GPIOC, GPIO14);
		vTaskDelay(pdMS_TO_TICKS(1500));
	}

}
