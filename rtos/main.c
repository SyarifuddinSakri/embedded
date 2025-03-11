#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/i2c.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

void clock_setup(void);
void gpio_setup(void);
void led1_task(void *pvParameters);
void led2_task(void *pvParameters);

int main(void) {
	clock_setup();
	gpio_setup();

	xTaskCreate(led1_task, "LED 1", 128, (void*)300, 1, NULL);
	xTaskCreate(led2_task, "LED 2", 128, (void*)4000, 1, NULL);

	while(1){
		//Code shoduld not reach here
	}

}
void clock_setup(void){
    //General clock.
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
    //Enable clock for SPI1, USART1 through GPIOA
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART1);
    rcc_periph_clock_enable(RCC_SPI1);
    //Enable clock for SPI2, through GPIOB
    rcc_periph_clock_enable(RCC_SPI2);
    rcc_periph_clock_enable(RCC_GPIOB);
    //Enable clock for GPIOC
    rcc_periph_clock_enable(RCC_GPIOC);
}

void gpio_setup(void){
		gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12 | GPIO13);
}
void led1_task(void *pvParameters){
	uint32_t delay  = *(uint32_t*)pvParameters;

	while(1){
		gpio_toggle(GPIOC, GPIO13);
		vTaskDelay(delay);
	}
}
void led2_task(void *pvParameters){
	uint32_t delay  = *(uint32_t*)pvParameters;

	while(1){
		gpio_toggle(GPIOC, GPIO12);
		vTaskDelay(delay);
	}
}
