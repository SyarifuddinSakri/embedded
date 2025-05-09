#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <stdint.h>
#include "wizchip_conf.h"
#include "spi_w5500.h"
#include "httpServer.h"
#include "log.h"

uint8_t SPIReadWrite(uint8_t data){
    //Wait for the SPI 1 Status Register Transmit is empty
    while((SPI_SR(SPI1)&SPI_SR_TXE)!= SPI_SR_TXE){
    }

    SPI1_DR=data;

    //Wait until data returned from the peripheral
    while(!(SPI_SR(SPI1)&SPI_SR_RXNE)){
    }
    return SPI1_DR;
}

void wizchip_select(void){
    gpio_clear(GPIOA, GPIO4);
}

void wizchip_deselect(void){
    gpio_set(GPIOA, GPIO4);
}

uint8_t wizchip_read(void){
    uint8_t rb;
    rb=SPIReadWrite(0x00);
    return rb;
}

void wizchip_write(uint8_t wb){
    SPIReadWrite(wb);
}

void wizchip_readburst(uint8_t* pBuf, uint16_t len){
    for(uint16_t i=0; i<len; i++){
        pBuf[i] = SPIReadWrite(0x00);
    }
}

void wizchip_writeburst(uint8_t* pBuf, uint16_t len){
    for(uint16_t i=0; i<len; i++){
        SPIReadWrite(pBuf[i]);
    }
}

void mcu_reset(void){
	my_printf("MCU_reset called\n");
}

void wdt_reset(void){
	my_printf("WDT_reset called\n");
}

void W5500Init(void){
    uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

    spi_set_nss_high(SPI1);

    gpio_clear(GPIOA, GPIO3);
    gpio_set(GPIOA, GPIO3);

    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
    reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);

		//http callback function
		reg_httpServer_cbfunc(mcu_reset, wdt_reset);

    if(ctlwizchip(CW_INIT_WIZCHIP, (void*)memsize)==-1){
        while(1);
    }
}
