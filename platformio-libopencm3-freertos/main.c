#include "FreeRTOS.h"
#include "sysout.h"
#include "task.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

void vApplicationStackOverflowHook(
	TaskHandle_t xTask __attribute__((unused)),
    char *pcTaskName __attribute__((unused))) {

	for (;;);
}

static void task1(void *args __attribute__((unused))) {
    for (;;) {
		gpio_toggle(GPIOC, GPIO13);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void task2(void *args __attribute__((unused))) {
    for (;;) {
		gpio_toggle(GPIOC, GPIO14);
		vTaskDelay(pdMS_TO_TICKS(1500));
	}
}

static void uart_task(void* args __attribute__((unused))){
	for(;;){
		sysoutln("makan nsi");
		vTaskDelay(pdMS_TO_TICKS(200));
		taskYIELD();
	}
}

int main(void) {
    // Setup main clock, using external 8MHz crystal 
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

    // Enable clock for GPIO channel C
    rcc_periph_clock_enable(RCC_GPIOC);

    // Set pinmode for PC13
	gpio_set_mode( GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	gpio_set_mode( GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO14);
	enable_sysout();

	// Turn LED off
	gpio_set(GPIOC, GPIO13);
	gpio_clear(GPIOC, GPIO14);

    // Tell FreeRTOS about our toggle task, and set it's stack and priority
	xTaskCreate(task1, "LED1", 100, NULL, 2, NULL);
	xTaskCreate(task2, "LED2", 100, NULL, 2, NULL);
	xTaskCreate(uart_task, "UART", 100, NULL, 2, NULL);

    // Start RTOS Task scheduler
	vTaskStartScheduler();

    // The task scheduler is blocking, so we should never come here...
	for (;;);
	return 0;
}
