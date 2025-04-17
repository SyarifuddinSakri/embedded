#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/spi.h>
#include "esp8266.h"
#include "libopencm3/stm32/f1/gpio.h"
#include "log.h"

void clock_setup(void);
void gpio_setup(void);
void uart1_setup(void);
void uart2_setup(void);
void spi1_setup(void);

int main(void) {
	clock_setup();
	gpio_setup();
	uart1_setup();
	uart2_setup();
	spi1_setup();

	my_printf("Program Start\r\n");
	/*esp_send_string("AT\r\n");*/
	esp8266_connect();
	while(1){
		for(int i=0; i<10000000; i++){
			__asm__("nop");
		}
		gpio_toggle(GPIOC, GPIO13);
	}
}

void clock_setup(void){
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOC);
	// Enable clock for GPIOA (USART1 and SPI1)
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_USART2);
	rcc_periph_clock_enable(RCC_SPI1);

}

void gpio_setup(void){
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13 | GPIO14);
	//Select and reset pin of the W5500
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4 | GPIO3);
	//Deselect the SPI
	gpio_set(GPIOA, GPIO4);
}

void uart1_setup(void) {
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
void uart2_setup(void) {
    // Configure TX (PA02) as alternate function push-pull
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

    // Configure RX (PA03) as input floating
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);

    // Set up USART2 parameters
    usart_set_baudrate(USART2, 115200);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    // Enable USART2
    usart_enable(USART2);
}
void spi1_setup(void){

    // Configure SPI1 pins:SCK, MOSI
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  GPIO_SPI1_SCK | GPIO_SPI1_MOSI);
    // Configure SPI1 pins:MISO
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_FLOAT,
                  GPIO_SPI1_MISO);

    //reset SPI, SPI_CR1 register cleared, SPI is disabled
    rcc_periph_reset_pulse(RST_SPI1);

    // Configure SPI1 parameters
    spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_256,
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);

    //enable NSS
    spi_enable_software_slave_management(SPI1);
    spi_set_nss_high(SPI1);

    // Enable SPI1
    spi_enable(SPI1);

}
