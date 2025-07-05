#include "print.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

void uart1_setup(void) {
  // Enable clock for USART1 and GPIOA
  rcc_periph_clock_enable(RCC_USART1);
  rcc_periph_clock_enable(RCC_GPIOA);

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

  // Enable USART1 interrupt
  //  usart_enable_rx_interrupt(USART1);
  //  nvic_enable_irq(NVIC_USART1_IRQ);

  // Enable USART1
  usart_enable(USART1);
}

int _write(char *ptr, int len) {
  for (int i = 0; i < len; i++) {
    if (ptr[i] == '\n') {
      usart_send_blocking(USART1, '\r');
    }
    usart_send_blocking(USART1, ptr[i]);
  }
  return len;
}
