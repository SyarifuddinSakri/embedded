#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include "FreeRTOS.h"
#include "portmacro.h"
#include "projdefs.h"
#include "task.h"
#include "semphr.h"
#include "log.h"

SemaphoreHandle_t mutex;

void task1(void* args __attribute((unused)));
void task2(void* args __attribute((unused)));
void uart_setup(void);
void gpio_setup(void);
void clock_setup(void);

int main(void) {
	clock_setup();
	gpio_setup();
	uart_setup();

	mutex = xSemaphoreCreateMutex();
	if(mutex==NULL){
		my_printf("Failed to create mutex\r\n");
	}else{
		my_printf("Success mutex creation\r\n");
	}

	xTaskCreate(task1, "LED1", 128, NULL, 1, NULL);
	xTaskCreate(task2, "LED2", 128, NULL, 1, NULL);

	vTaskStartScheduler();

	for(;;);

}

void vApplicationStackOverflowHook(TaskHandle_t xTask __attribute((unused)), char *pcTaskName){
				my_printf("Stack overflow in task %s\n", pcTaskName);
				while(1); //Halt or reset
}

void task1(void* args __attribute((unused))){

	for(;;){
		if(xSemaphoreTake(mutex, portMAX_DELAY)){
			gpio_toggle(GPIOC, GPIO13);
			my_printf("Toggling GPIO13\r\n");
			vTaskDelay(pdMS_TO_TICKS(1000));
			xSemaphoreGive(mutex);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}

}

void task2(void* args __attribute((unused))){

	for(;;){
		if(xSemaphoreTake(mutex, portMAX_DELAY)){
			gpio_toggle(GPIOC, GPIO14);
			my_printf("Toggling GPIO14\r\n");
			vTaskDelay(pdMS_TO_TICKS(300));
			xSemaphoreGive(mutex);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}

}

void clock_setup(void){
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOC);
	// Enable clock for GPIOA (USART1 and SPI1)
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_SPI1);

}

void gpio_setup(void){
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13 | GPIO14);
	//Reset pin of the W5500
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);
	gpio_set(GPIOA, GPIO4);
}

void uart_setup(void) {
    // Configure TX (PA9) as alternate function push-pull
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO9);

    // Configure RX (PA10) as input floating
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_FLOAT, GPIO10);

    // Set up USART1 parameters
    usart_set_baudrate(USART1, 115200);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

    // Enable USART1
    usart_enable(USART1);
}
