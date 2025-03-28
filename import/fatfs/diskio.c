#include "ff.h"
#include "diskio.h"
#include "sd_spi.h"
#include "log.h"

DSTATUS disk_initialize(BYTE pdrv) {
    return (sd_init() == 0) ? 0 : STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv) {
    return 0; // Assume OK
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
    return (sd_read_sector(sector, buff, count) == 0) ? RES_OK : RES_ERROR;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
    my_printf("Reading sector %lu\r\n", sector);
    if (sd_read_sector(sector, buff, count) != 0) {
        my_printf("Read failed!\r\n");
        return RES_ERROR;  // This causes FR_DISK_ERR
    }
    return RES_OK;
}
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
            
        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;  // MUST be 512 for FatFs
            return RES_OK;
            
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;   // Erase block size in sectors
            return RES_OK;
            
        case GET_SECTOR_COUNT: {
            // For non-SDHC cards, you may need to read this from CSD
            // Temporary workaround for testing:
            *(DWORD*)buff = 0x100000; // 1GB card (adjust for your card)
            return RES_OK;
        }
            
        default:
            return RES_PARERR;
    }
}
