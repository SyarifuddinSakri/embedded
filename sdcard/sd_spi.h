#pragma once
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>

// SD Card Commands (SPI Mode)
#define SD_CMD0   (0x40 | 0)    // GO_IDLE_STATE
#define SD_CMD8   (0x40 | 8)    // SEND_IF_COND
#define SD_CMD16  (0x40 | 16)   // SET_BLOCKLEN
#define SD_CMD17  (0x40 | 17)   // READ_SINGLE_BLOCK
#define SD_CMD24  (0x40 | 24)   // WRITE_BLOCK
#define SD_ACMD41 (0xC0 | 41)   // APP_SEND_OP_COND (SDC)
#define SD_CMD58  (0x40 | 58)   // READ_OCR

// SD Card Responses
#define SD_R1_IDLE        0x01
#define SD_R1_ILLEGAL_CMD 0x04

// Prototypes
void sd_spi_init(void);
uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg);
uint8_t sd_init(void);
uint8_t sd_read_sector(uint32_t sector, uint8_t *data, uint32_t count);
uint8_t sd_write_sector(uint32_t sector, const uint8_t *data, uint32_t count);
