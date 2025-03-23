#include "spi.h"
#include "sd_spi.h"
#include <libopencm3/stm32/gpio.h>
#include <stdint.h>

#define CMD0  0x40  // GO_IDLE_STATE
#define CMD8  0x48  // SEND_IF_COND
#define CMD17 0x51  // READ_SINGLE_BLOCK
#define CMD24 0x58  // WRITE_BLOCK
#define CMD58 0x7A  // READ_OCR
#define ACMD41 0x69 // APP_CMD (with CMD41)

static void sd_send_command(uint8_t cmd, uint32_t arg) {
    spi_sd_write(cmd);
    spi_sd_write((arg >> 24) & 0xFF);
    spi_sd_write((arg >> 16) & 0xFF);
    spi_sd_write((arg >> 8) & 0xFF);
    spi_sd_write(arg & 0xFF);
    spi_sd_write(0x95);  // Dummy CRC for CMD0
}

int sd_init(void) {
    spi1_setup();
    spi_sd_deselect();
    
    for (int i = 0; i < 10; i++) spi_sd_read();  // Send 80 clocks to wake up SD card

    spi_sd_select();
    sd_send_command(CMD0, 0);
    while (spi_sd_read() != 0x01);  // Wait for IDLE state
    spi_sd_deselect();

    spi_sd_select();
    sd_send_command(CMD8, 0x000001AA);
    if (spi_sd_read() != 0x01) return -1;
    spi_sd_deselect();

    while (1) {
        spi_sd_select();
        sd_send_command(ACMD41, 0x40000000);
        if (spi_sd_read() == 0) break;
        spi_sd_deselect();
    }
    return 0; // Success
}

int sd_read_sector(uint32_t sector, uint8_t *buffer) {
    spi_sd_select();
    sd_send_command(CMD17, sector);
    while (spi_sd_read() != 0xFE);  // Wait for data token

    spi_sd_readburst(buffer, 512);

    spi_sd_read();  // Ignore CRC
    spi_sd_read();
    spi_sd_deselect();
    return 0;
}

int sd_write_sector(uint32_t sector, const uint8_t *buffer) {
    spi_sd_select();
    sd_send_command(CMD24, sector);
    spi_sd_read();  // Ignore response

    spi_sd_write(0xFE);  // Start token
    spi_sd_writeburst((uint8_t*)buffer, 512);

    spi_sd_read();  // Ignore CRC
    spi_sd_read();
    spi_sd_deselect();
    return 0;
}

uint32_t sd_get_sector_count(void) {
    return 32768;  // Assume 16MB SD card
}
