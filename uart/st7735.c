#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <stdint.h>
#include "st7735.h"
#include "blink.h"
#include "sysout.h"

void st7735_reset(void) {
    gpio_clear(GPIOB, ST7735_RST);
    delay(100);
    gpio_set(GPIOB, ST7735_RST);
    delay(100);
}
uint8_t st7735_read_write(uint8_t data){
    //Wait for the SPI 2 Status Register Transmit is empty
    while((SPI_SR(SPI2)&SPI_SR_TXE)!= SPI_SR_TXE){
    }

    // Check for SPI errors
    if (SPI_SR(SPI2) & SPI_SR_OVR) {
        /*sysout("SPI Overrun Error");*/
        return 0xFF;
    }
    if (SPI_SR(SPI2) & SPI_SR_MODF) {
        /*sysout("SPI Mode Fault");*/
        return 0xFF;
    }

    SPI2_DR=data;
    /*spi_send(SPI2, data);*/

    //Wait until data returned from the peripheral
    while(!(SPI_SR(SPI2)&SPI_SR_RXNE)){
    }
    return SPI2_DR;
}

void st7735_write_command(uint8_t cmd) {
    gpio_clear(GPIOB, ST7735_DC);  // Command mode
    gpio_clear(GPIOB, ST7735_CS);  // Select display
    st7735_read_write(cmd);
    gpio_set(GPIOB, ST7735_CS);    // Deselect display
}

void st7735_write_data(uint8_t data) {
    gpio_set(GPIOB, ST7735_DC);  // Data mode
    gpio_clear(GPIOB, ST7735_CS);
    st7735_read_write(data);
    gpio_set(GPIOB, ST7735_CS);
}

void st7735_write_burst(uint8_t cmd, uint8_t* data, uint8_t len){
    gpio_clear(GPIOB, ST7735_DC);//Switch to command mode
    gpio_clear(GPIOB, ST7735_CS);//Chip select
    st7735_read_write(cmd);//Write command
    gpio_set(GPIOB, ST7735_DC);//Switch to data mode
    for (int i=0; i<len; i++) {
        st7735_read_write(data[i]);//Send the data
    }
    gpio_set(GPIOB, ST7735_CS);
}

void st7735_init(void) {
    st7735_reset();
    
    st7735_write_command(0x01);  // Software reset
    delay(10000);

    st7735_write_command(0x11);  // Sleep Out
    delay(1000);

    st7735_write_burst(0x3A, (uint8_t[]){0x05}, 1); //Defines color mode
    
    st7735_write_burst(0x36, (uint8_t[]){0xC0}, 1); //memory data acces control 0x00(normal)

    st7735_write_command(0x13);//Normal display mode on

    st7735_write_command(0x29);  // Display On
}

void set_addr_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
    st7735_write_command(0x2A);
    st7735_write_data(x >> 8);
    st7735_write_data(x & 0xFF);
    st7735_write_data((x + w - 1) >> 8);
    st7735_write_data((x + w - 1) & 0xFF);

    st7735_write_command(0x2B);
    st7735_write_data(y >> 8);
    st7735_write_data(y & 0xFF);
    st7735_write_data((y + h - 1) >> 8);
    st7735_write_data((y + h - 1) & 0xFF);
}

void draw_pixel(uint16_t x, uint16_t y, uint16_t color){
    if (x >= ST77XX_WIDTH || y >= ST77XX_HEIGHT) return;

    set_addr_window(x, y, 1, 1);
    st7735_write_command(0x2C);
    st7735_write_data(color >> 8);
    st7735_write_data(color & 0xFF);
}

// Fill the screen with a single color
void st77xx_fill_screen(uint16_t color) {
    set_addr_window(0, 0, ST77XX_WIDTH, ST77XX_HEIGHT);
    st7735_write_command(0x2C);

    for (uint32_t i = 0; i < (ST77XX_WIDTH * ST77XX_HEIGHT); i++) {
        st7735_write_data(color >> 8);
        st7735_write_data(color & 0xFF);
    }
}
