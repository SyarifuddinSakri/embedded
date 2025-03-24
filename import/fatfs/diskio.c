#include <stdint.h>
#include "diskio.h"
#include "spi.h"
#include "sd_spi.h"

#define SD_CMD0  0x40  /* GO_IDLE_STATE */
#define SD_CMD8  0x48  /* SEND_IF_COND */
#define SD_CMD55 0x77  /* APP_CMD */
#define SD_ACMD41 0x69 /* SD_SEND_OP_COND */
#define SD_CMD58 0x7A  /* READ_OCR */
#define DEV_MMC 1

static void sd_spi_send_command(uint8_t cmd, uint32_t arg) {
    spi_sd_transfer(cmd | 0x40);
    spi_sd_transfer((arg >> 24) & 0xFF);
    spi_sd_transfer((arg >> 16) & 0xFF);
    spi_sd_transfer((arg >> 8) & 0xFF);
    spi_sd_transfer(arg & 0xFF);
    spi_sd_transfer(0x95); // Dummy CRC for CMD0
}

DSTATUS disk_initialize(BYTE pdrv) {
    if (pdrv != DEV_MMC) return STA_NOINIT;

    spi1_setup();
    spi_sd_deselect();
    for (uint8_t i = 0; i < 10; i++) spi_sd_transfer(0xFF);
    
    spi_sd_select();
    sd_spi_send_command(SD_CMD0, 0);
    spi_sd_deselect();
    spi_sd_transfer(0xFF);

    return RES_OK;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
    if (pdrv != DEV_MMC) return RES_PARERR;

    spi_sd_select();
    sd_spi_send_command(0x51, sector * 512);
    for (UINT i = 0; i < count * 512; i++) buff[i] = spi_sd_transfer(0xFF);
    spi_sd_deselect();
    
    return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
    if (pdrv != DEV_MMC) return RES_PARERR;

    spi_sd_select();
    sd_spi_send_command(0x58, sector * 512);
    for (UINT i = 0; i < count * 512; i++) spi_sd_transfer(buff[i]);
    spi_sd_deselect();

    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    if (pdrv != DEV_MMC) return RES_PARERR;
    
    switch (cmd) {
    case CTRL_SYNC:
        return RES_OK;
    case GET_SECTOR_COUNT:
        *(DWORD *)buff = 1024; // Example size
        return RES_OK;
    }
    return RES_PARERR;
}

DSTATUS disk_status(BYTE pdrv) {
    if (pdrv != DEV_MMC) return STA_NOINIT;
    return 0;
}
