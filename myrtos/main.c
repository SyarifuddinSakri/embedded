#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include "FreeRTOS.h"
#include "libopencm3/stm32/f1/gpio.h"
#include "projdefs.h"
#include "task.h"

void task1(void* args __attribute((unused)));
void task2(void* args __attribute((unused)));
void uart_setup(void);
void task_uart(void* args __attribute((unused)));
void uart_send_string(const char *str);

int main(void) {
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13 | GPIO14);
	uart_setup();

	xTaskCreate(task1, "LED1", 128, NULL, 1, NULL);
	xTaskCreate(task2, "LED2", 128, NULL, 1, NULL);
	xTaskCreate(task_uart, "UART", 128, NULL, 1, NULL);

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
        uart_send_string("Blink from pin 14\r\n");
		vTaskDelay(pdMS_TO_TICKS(1500));
	}

}
void uart_send_string(const char *str) {
    while (*str) {
        usart_send_blocking(USART1, *str++);
    }
}
void task_uart(void* args __attribute((unused))) {
    for (;;) {
        uart_send_string("Hello from STM32!\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void uart_setup(void) {
    // Enable clock for USART1 and GPIOA
    rcc_periph_clock_enable(RCC_USART1);
    rcc_periph_clock_enable(RCC_GPIOA);

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
