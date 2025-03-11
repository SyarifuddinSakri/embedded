#include "setup.h"
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define CS_7735 GPIO12 // PB12 - Chip select
#define DC_7735 GPIO10 // PB10 - Data/Command
#define RST_7735 GPIO11 // PB11 - Reset

void clock_init(void){
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	
	//Enable clock for SPI2 and GPIOB
	rcc_periph_clock_enable(RCC_SPI2);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);

}

void spi2_init(void){

	//Configure SPI2 SCK (PB13) and MOSI (PB15) as alternate function push pull
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO13 | GPIO15);

	//Configure CS, DC, and RESET as outputs
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, CS_7735 | DC_7735 | RST_7735);


	/*Initialize SPI2
	* set spi clock to 9MHZ (72MHZ/8)
	* CPOL = 0
	* CPHA = 0 (SPI MODE 0)
	* 8-bit data frame
	* MSB First
	*/
	spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_8, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);

	//Set NSS management software
	spi_enable_software_slave_management(SPI2);
	spi_set_nss_high(SPI2);

	spi_enable(SPI2);

}

void	st7735_send_command(uint8_t cmd){
	gpio_clear(GPIOB, DC_7735);//Command mode
	gpio_clear(GPIOB, CS_7735); //Select display
	spi_send(SPI2, cmd);
	while(!(SPI_SR(SPI2) & SPI_SR_TXE));
	gpio_set(GPIOB, CS_7735);
}

void st7735_send_data(uint8_t data) {
    gpio_set(GPIOB, DC_7735); // Data mode
    gpio_clear(GPIOB, CS_7735); // Select display
    spi_send(SPI2, data);
    while (!(SPI_SR(SPI2) & SPI_SR_TXE));
    gpio_set(GPIOB, CS_7735);
}

void st7735_reset(void){
	gpio_clear(GPIOB, RST_7735);
	for (volatile int i=0; i<100000; i++);
	gpio_set(GPIOB, RST_7735);
}

void st7735_init(void){
	st7735_reset();

	st7735_send_command(0x01); //Software reset
	for (volatile int i=0; i<100000; i++);
	
	st7735_send_command(0x11); //Sleep out
	for (volatile int i=0; i<100000; i++);
	
	st7735_send_command(0x29); //Display on
}
