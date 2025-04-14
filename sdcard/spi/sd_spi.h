
#ifndef SPI_W5500_H
#define SPI_W5500_H
#include "ff.h"
#include <stdint.h>
// SD Card Commands (SPI mode)
// Standard SD Commands (CMDn)
#define CMD0    0   // GO_IDLE_STATE: Resets the card
#define CMD1    1   // SEND_OP_COND (MMC cards only)
#define CMD8    8   // SEND_IF_COND: Check voltage range
#define CMD9    9   // SEND_CSD: Read CSD register
#define CMD10   10  // SEND_CID: Read CID register
#define CMD12   12  // STOP_TRANSMISSION: Stop multi-block read/write
#define CMD13   13  // SEND_STATUS: Card status
#define CMD16   16  // SET_BLOCKLEN: Set block size (usually 512)
#define CMD17   17  // READ_SINGLE_BLOCK
#define CMD18   18  // READ_MULTIPLE_BLOCK
#define CMD24   24  // WRITE_BLOCK
#define CMD25   25  // WRITE_MULTIPLE_BLOCK
#define CMD27   27  // PROGRAM_CSD
#define CMD55   55  // APP_CMD: Indicates following command is ACMD
#define CMD58   58  // READ_OCR: Read OCR register
#define CMD59   59  // CRC_ON_OFF (optional)

// Application-Specific Commands (ACMDn — always sent after CMD55)
#define ACMD13  13  // SD_STATUS
#define ACMD22  22  // SEND_NUM_WR_BLOCKS
#define ACMD23  23  // SET_WR_BLK_ERASE_COUNT
#define ACMD41  41  // SD_SEND_OP_COND (initialize SD card)
#define ACMD42  42  // SET_CLR_CARD_DETECT

// Data Tokens for SPI transfers
#define DATA_TOKEN_START_BLOCK         0xFE  // Start of single block read/write
#define DATA_TOKEN_MULTI_WRITE_STOP    0xFD  // Stop transmission for multi-block write
#define DATA_TOKEN_DUMMY               0xFF  // Dummy byte / idle bus

// Data Response Tokens (write)
#define DATA_RESP_MASK                 0x1F
#define DATA_RESP_ACCEPTED             0x05
#define DATA_RESP_CRC_ERROR            0x0B
#define DATA_RESP_WRITE_ERROR          0x0D

// R1 Response Flags
#define R1_IDLE_STATE      (1 << 0)
#define R1_ILLEGAL_COMMAND (1 << 2)

// Data Tokens
#define DATA_START_BLOCK   0xFE  // Start of data block
#define DATA_ACCEPTED      0x05  // Write accepted
#define SD_COMMAND_TIMEOUT  5000
#define SUCCESS  0
#define ERROR    1
//
struct command_fields{
	uint8_t start_bit;
	uint8_t transmitter_bit; //start bit and transmitter bit will make up for '01'
	uint8_t index;
	uint32_t argument;
	uint8_t crc;
	uint8_t end_bit;
};
#define SDCARD_FAIL     0
#define SDCARD_V1       1
#define SDCARD_V2       2
#define SDCARD_MMCV3    3
#define SDCARD_SDC      4  // Flag indicating SD card (not MMC)
#define SDCARD_BLOCK    10

uint8_t SPIReadWrite(uint8_t data);
void sd_select(void);
void sd_deselect(void);
uint8_t sd_read(void);
void sd_write(uint8_t wb);
void sd_readburst(uint8_t* pBuf, uint16_t len);
void sd_writeburst(uint8_t* pBuf, uint16_t len);
uint8_t SD_CARD_InitialiseCard(void);
uint8_t SD_CARD_InitialiseCardV1(void);
uint8_t SD_CARD_InitialiseCardV2(void);
uint32_t SD_CARD_Cmd(UINT cmd, UINT arg);
void SD_CARD_WriteCom(struct command_fields *com);
UINT SD_CARD_Read(BYTE *buffer, UINT length);
UINT SD_CARD_Write(const BYTE *buffer, BYTE token);
uint8_t SD_CARD_RecieveR1(void);
uint32_t SD_CARD_RecieveR7(void);
uint32_t SD_CARD_RecieveR3(void);
#endif
