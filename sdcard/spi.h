#include <stdint.h>
#ifndef SPI_H
#define SPI_H

uint8_t spi_sd_transfer(uint8_t data);
void spi_sd_select(void);
void spi_sd_deselect(void);
uint8_t spi_sd_read(void);
void spi_sd_write(uint8_t wb);
void spi_sd_readburst(uint8_t* pBuf, uint16_t len);
void spi_sd_writeburst(uint8_t* pBuf, uint16_t len);
void spi1_setup(void);

#endif
