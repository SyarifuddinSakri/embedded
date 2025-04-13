
#ifndef SPI_W5500_H
#define SPI_W5500_H
#include <stdint.h>
// SD Card Commands (SPI mode)
#define CMD0   0   // GO_IDLE_STATE (reset)
#define CMD8   8   // SEND_IF_COND (check voltage)
#define CMD16  16  // SET_BLOCKLEN (set sector size, usually 512 bytes)
#define CMD17  17  // READ_SINGLE_BLOCK
#define CMD24  24  // WRITE_BLOCK
#define CMD55  55  // APP_CMD (prefix for ACMD commands)
#define CMD58  58  // READ_OCR (check card voltage)
#define ACMD41 41  // SD_SEND_OP_COND (init card)

// R1 Response Flags
#define R1_IDLE_STATE      (1 << 0)
#define R1_ILLEGAL_COMMAND (1 << 2)

// Data Tokens
#define DATA_START_BLOCK   0xFE  // Start of data block
#define DATA_ACCEPTED      0x05  // Write accepted

uint8_t SPIReadWrite(uint8_t data);
void sd_select(void);
void sd_deselect(void);
uint8_t sd_read(void);
void sd_write(uint8_t wb);
void sd_readburst(uint8_t* pBuf, uint16_t len);
void sd_writeburst(uint8_t* pBuf, uint16_t len);
uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg);
#endif
