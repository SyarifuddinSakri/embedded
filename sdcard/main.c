#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>
#include "libopencm3/stm32/f1/gpio.h"
#include "ff.h"
#include "diskio.h"
#include "log.h"
#include "spi.h"

void clock_setup(void);
void uart_setup(void);
void gpio_init(void);

int main(void){
	clock_setup();
	uart_setup();
	gpio_init();
	spi1_setup();

	FATFS FatFs;
	FIL fil;
	FRESULT fres;

	my_printf("disk_status(1) -> %d\r\n",disk_status(1));
	my_printf("mounting\r\n");
	fres = f_mount(&FatFs, "1:", 1); //1=mount now
	my_printf("Tried Mounting\r\n");
	if(fres != FR_OK){
		my_printf("Mounting Error, Code : %d\r\n", fres);
	}
	my_printf("Finished mounting\r\n");

	while (1) {
		for (int i=0; i<1000000; i++) {
			__asm__("nop");
		}
		gpio_toggle(GPIOC, GPIO13);
	}

	return 0;
}

void clock_setup(void){
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOC);
	// Enable clock for GPIOA (USART1 and SPI1)
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_SPI1);

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

void gpio_init(void){
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13 | GPIO14);
}
