/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sdcard.h"
#include "log.h"
#include "spi.h"



/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    my_printf("Checking disk status for pdrv=%d\r\n", pdrv);

    if (pdrv == DEV_MMC) {
        my_printf("Returning disk status OK\r\n");
        return 0;  // SD card is available
    }

    my_printf("Invalid drive number: %d\r\n", pdrv);
    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    my_printf("Initializing disk pdrv=%d\r\n", pdrv);

    if (pdrv == DEV_MMC) {  // Ensure SD card (pdrv=1) is being used
        if (sd_init()) {
            my_printf("SD Init failed!\r\n");
            return STA_NOINIT;
        }
        my_printf("SD Init successful!\r\n");
        return 0; // Initialization OK
    }
    
    my_printf("Invalid drive number: %d\r\n", pdrv);
    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	    my_printf("disk read pdrv=%d, sec=%d, cnt=%d\r\n", pdrv, sector, count);

    if (pdrv != DEV_MMC) {
        return RES_PARERR; // Invalid drive
    }

    while (count) {
        if (sd_read_block(buff, sector) != 0) {  // Check for nonzero error return
            return RES_ERROR;
        }
        --count;
        ++sector;
        buff += 512;
    }

    my_printf("Read successful\r\n");
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
    my_printf("disk write pdrv=%d, sec=%lu, cnt=%d\r\n", pdrv, sector, count);

    if (pdrv != DEV_MMC) {
        return RES_PARERR; // Invalid drive
    }

    while (count) {
        if (sd_write_block((uint8_t*)buff, sector) != 0) {  // Check return value
            return RES_ERROR;
        }
        --count;
        ++sector;
        buff += 512;
    }

    return RES_OK;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    if (pdrv != DEV_MMC) {
        return RES_PARERR;
    }

    switch (cmd)
    {
        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            return RES_OK;
        case CTRL_SYNC:
            return RES_OK;  // If no pending writes
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 8;  // Typically, SD block size is 512 bytes, but adjust if needed
            return RES_OK;
        default:
            return RES_PARERR;
    }
}

