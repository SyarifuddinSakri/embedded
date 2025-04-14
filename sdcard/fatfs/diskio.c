#include "diskio.h"     /* FatFs lower layer API */
#include "sd_spi.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM     0   /* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC     1   /* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB     2   /* Example: Map USB MSD to physical drive 2 */

static volatile DSTATUS Stat = STA_NOINIT;  /* Disk status */

static BYTE CardType;   /* Card type flags (b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing) */



/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    if(pdrv) 
        return STA_NOINIT;      // Supports only drive 0

    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive nmuber to identify the drive */
)
{
    if (Stat & STA_NODISK) 
        return Stat;    /* No card in the socket? */

    //SLOW

    uint8_t ty = 0;
    ty = SD_CARD_InitialiseCard();

    CardType = ty;

    if(ty)
        Stat &= ~STA_NOINIT;

    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Start sector in LBA */
    UINT count      /* Number of sectors to read */
)
{
    //DRESULT res;
    //int result;
    if(pdrv || !count)
        return RES_PARERR;
    if (Stat & STA_NOINIT)
        return RES_NOTRDY;

    if(!(CardType & SDCARD_BLOCK))
        sector *= 512;

    if(count == 1)
    {
        if((SD_CARD_Cmd(CMD17, sector) == 0x00) &&  SD_CARD_Read(buff, 512))
            count = 0;
    }
    else
    {
        if(SD_CARD_Cmd(CMD18, sector) == 0)
        {
            do
            {
                if(!SD_CARD_Read(buff, 512))
                    break;
                buff += 512;
            }
            while(--count);
            SD_CARD_Cmd(CMD12, 0);
        }
    }

    return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to write */
)
{
    //DRESULT res;
    //int result;

    if (pdrv || !count)
        return RES_PARERR;
    if (Stat & STA_NOINIT)
        return RES_NOTRDY;
    if (Stat & STA_PROTECT)
        return RES_WRPRT;

    if(!(CardType & SDCARD_BLOCK))
        sector *= 512;

    if(count == 1)
    {
        if((SD_CARD_Cmd(CMD24, sector) == 0x00) &&  SD_CARD_Write(buff, 0xFE))
            count = 0;
    }
    else
    {
        if (CardType & SDCARD_SDC)
            SD_CARD_Cmd(ACMD23, count);

        if(SD_CARD_Cmd(CMD25, sector) == 0)
        {
            do
            {
                if(!SD_CARD_Write(buff, 0xFC))
                    break;

                buff += 512;
            }
            while(--count);

            if (!SD_CARD_Write(0, 0xFD))    /* STOP_TRAN token */
                count = 1;
        }
    }

    return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    //DRESULT res;
    //int result;

    //NOT NEEDED AT THE MOMENT

    return RES_PARERR;
}

DWORD get_fattime (void)
{
    /* Pack date and time into a DWORD variable */
    return    ((DWORD)(2017 - 1980) << 25)
            | ((DWORD)1 << 21)
            | ((DWORD)1 << 16)
            | ((DWORD)0 << 11)
            | ((DWORD)0 << 5)
            | ((DWORD)0 >> 1);
}
