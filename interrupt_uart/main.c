#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <stdint.h>
#include "log.h"

volatile char rx_char[128];
volatile uint32_t iter = 0;
void uart_setup(void);
int main(void) {
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	uart_setup();

  rcc_periph_clock_enable(RCC_GPIOC);
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                GPIO13);

	my_printf("starting the program\n");

	for(;;){
		gpio_toggle(GPIOC, GPIO13);
		for(int i=0; i<10000000; i++){
			__asm__("nop");
		}
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

		//Enable USART1 interrupt
		usart_enable_rx_interrupt(USART1);
		nvic_enable_irq(NVIC_USART1_IRQ);

    // Enable USART1
    usart_enable(USART1);
}
void usart1_isr(void){
	    // Check if RXNE (Receive Not Empty) is set
	char charbuf;
	if ((USART_SR(USART1) & USART_SR_RXNE) != 0) {
			charbuf = usart_recv(USART1);  // Read the received byte
			if(charbuf!='\n' && charbuf!='\r'){
			rx_char[iter++] = charbuf;
			//To prevent buffer overflow
				if(iter>=sizeof(rx_char)-1){
					rx_char[sizeof(rx_char)-1] = '\0';
					iter = 0;
					return;
				}
			}else{
			rx_char[iter] = '\0';
			iter = 0;
			my_printf("%s\n",rx_char);
			return;
		}
	}
}
