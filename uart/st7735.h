#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>
#define ST7735_CS   GPIO12  // Chip Select (PB12)
#define ST7735_RST  GPIO10  // Reset (PB10)
#define ST7735_DC   GPIO11  // Data/Command (PB11)
#define ST77XX_WIDTH  128
#define ST77XX_HEIGHT 165

void st7735_reset(void);
uint8_t st7735_read_write(uint8_t data);
void st7735_write_command(uint8_t cmd);
void st7735_write_data(uint8_t data);
void st7735_write_burst(uint8_t cmd, uint8_t* data, uint8_t len);
void st7735_init(void);
void set_addr_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void st77xx_fill_screen(uint16_t color);

#endif
