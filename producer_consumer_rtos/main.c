#include "FreeRTOS.h"
#include "log.h"
#include "portmacro.h"
#include "projdefs.h"
#include "queue.h"
#include "task.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#define QUEUE_LENGTH 5
#define QUEUE_ITEM_SIZE sizeof(int)

QueueHandle_t xQueue;

void vProducer_task(void *args __attribute((unused)));
void vConsumer_task(void *args __attribute((unused)));
void uart_setup(void);
void clock_setup(void);
void gpio_setup(void);

int main(void) {
  clock_setup();
  gpio_setup();
  uart_setup();

  xQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
  if (!xQueue) {
    my_printf("cannot create xQueue\n");
    while (1)
      ; // halt the program
  }

  xTaskCreate(vProducer_task, "Task1", 128, NULL, 1, NULL);
  xTaskCreate(vConsumer_task, "Task2", 128, NULL, 1, NULL);

  vTaskStartScheduler();

  for (;;)
    ;

  return 0;
}

void vProducer_task(void *args __attribute((unused))) {

  int value = 0;
  for (;;) {
    value++;
    if (xQueueSend(xQueue, &value, portMAX_DELAY) == pdPASS) {
      my_printf("Produced %d\n", value);
    } else {
      my_printf("Queue full, can't produce\n");
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void vConsumer_task(void *args __attribute((unused))) {

  int received;
  for (;;) {
    if (xQueueReceive(xQueue, &received, portMAX_DELAY) == pdPASS) {
      my_printf("Consumed %d\n", received);
    }
  }
}

void clock_setup(void) {
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
  rcc_periph_clock_enable(RCC_GPIOC);
  // Enable clock for USART1 and GPIOA
  rcc_periph_clock_enable(RCC_USART1);
  rcc_periph_clock_enable(RCC_GPIOA);
}

void gpio_setup(void) {
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                GPIO13 | GPIO14);
}
void uart_setup(void) {
  // Configure TX (PA9) as alternate function push-pull
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                GPIO9);

  // Configure RX (PA10) as input floating
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO10);

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
