#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <stdint.h>
#include "spi.h"
#include "log.h"

uint8_t spi_sd_transfer(uint32_t spi, uint8_t data){
    //Wait for the SPI 1 Status Register Transmit is empty
		/*my_printf("Transferring data %d\r\n", data);*/
    while((SPI_SR(spi)&SPI_SR_TXE)!= SPI_SR_TXE){
    }

		SPI_DR(spi)=data;

    //Wait until data returned from the peripheral
    while(!(SPI_SR(spi)&SPI_SR_RXNE)){
    }
    return SPI_DR(spi);
}

void spi_sd_select(void){
    gpio_clear(GPIOA, GPIO4);
}

void spi_sd_deselect(void){
    gpio_set(GPIOA, GPIO4);
}

uint8_t spi_sd_read(uint32_t spi){
    uint8_t rb;
    rb=spi_sd_transfer(spi,0x00);
    return rb;
}

void spi_sd_write(uint32_t spi, uint8_t wb){
    spi_sd_transfer(spi, wb);
}

void spi_sd_readburst(uint32_t spi, uint8_t* pBuf, uint16_t len){
    for(uint16_t i=0; i<len; i++){
        pBuf[i] = spi_sd_transfer(spi, 0x00);
    }
}

void spi_sd_writeburst(uint32_t spi, uint8_t* pBuf, uint16_t len){
    for(uint16_t i=0; i<len; i++){
        spi_sd_transfer(spi, pBuf[i]);
    }
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
	spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_64,
									SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
									SPI_CR1_CPHA_CLK_TRANSITION_2,
									SPI_CR1_DFF_8BIT,
									SPI_CR1_MSBFIRST);
	//enable NSS
	spi_enable_software_slave_management(SPI1);
	spi_set_nss_high(SPI1);
	// Enable SPI1
	spi_enable(SPI1);

	my_printf("SPI set up\r\n");
}
