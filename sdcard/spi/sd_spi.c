#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include "sd_spi.h"
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

void sd_select(void){
    gpio_clear(GPIOA, GPIO4);
}

void sd_deselect(void){
    gpio_set(GPIOA, GPIO4);
}

uint8_t sd_read(void){
    uint8_t rb;
    rb=SPIReadWrite(0x00);
    return rb;
}

void sd_write(uint8_t wb){
    SPIReadWrite(wb);
}

void sd_readburst(uint8_t* pBuf, uint16_t len){
    for(uint16_t i=0; i<len; i++){
        pBuf[i] = SPIReadWrite(0x00);
    }
}

void sd_writeburst(uint8_t* pBuf, uint16_t len){
    for(uint16_t i=0; i<len; i++){
        SPIReadWrite(pBuf[i]);
    }
}

uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg) {
    uint8_t res;
    uint8_t retries = 0;

    // Wait for card to be ready (send dummy bytes)
    sd_write(0xFF);

    // Send command (6 bytes: command + arg + CRC)
    sd_write(0x40 | cmd);             // Start bit (0x40) + command
    sd_write((arg >> 24) & 0xFF);     // Argument (MSB first)
    sd_write((arg >> 16) & 0xFF);
    sd_write((arg >> 8) & 0xFF);
    sd_write(arg & 0xFF);

    // CRC (dummy for most commands except CMD0 and CMD8)
    uint8_t crc = 0xFF;
    if (cmd == CMD0) crc = 0x95;      // Valid CRC for CMD0
    if (cmd == CMD8) crc = 0x87;      // Valid CRC for CMD8
    sd_write(crc);

    // Wait for response (up to 8 tries)
    while ((res = sd_read()) & 0x80) {  // Response bit 7 = 0 means valid
        if (retries++ > 8) break;
    }

    return res;  // Returns R1 response (e.g., 0x00 = success)
}
