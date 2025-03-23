#ifndef SD_SPI_H
#define SD_SPI_H

#include <stdint.h>

int sd_init(void);
int sd_read_sector(uint32_t sector, uint8_t *buffer);
int sd_write_sector(uint32_t sector, const uint8_t *buffer);
uint32_t sd_get_sector_count(void);

#endif
