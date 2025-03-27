
#ifndef SPI_W5500_H
#define SPI_W5500_H
#include <stdint.h>
uint8_t SPIReadWrite(uint8_t data);
void wizchip_select(void);
void wizchip_deselect(void);
uint8_t wizchip_read(void);
void wizchip_write(uint8_t wb);
void wizchip_readburst(uint8_t* pBuf, uint16_t len);
void wizchip_writeburst(uint8_t* pBuf, uint16_t len);
void W5500Init(void);
void wizchip_io_enCritical(void);
void wizchip_io_exCritical(void);
#endif
