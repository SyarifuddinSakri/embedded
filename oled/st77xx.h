#ifndef ST77XX_H
#define ST77XX_H

#include <stdint.h>
#define ST77XX_CS   GPIO12  // Chip Select (PB12)
#define ST77XX_RST  GPIO10  // Reset (PB10)
#define ST77XX_DC   GPIO11  // Data/Command (PB11)
#define ST77XX_WIDTH  128
#define ST77XX_HEIGHT 170
#define ST77XX_SOFTWARE_RESET 0x01
#define ST77XX_SLEEP_IN 0x10
#define ST77XX_SLEEP_OUT 0x11
#define ST77XX_NORMAL_DISP_MODE 0x13
#define ST77XX_DISP_INVERSION_EN 0x21
#define ST77XX_DISP_INVERSION_DIS 0x21
#define ST77XX_SLCT_GAMMA_CURVE 0x26
#define ST77XX_DISP_OFF 0x28
#define ST77XX_DISP_ON 0x29
#define ST77XX_SET_COL_ADDR 0x2A
#define ST77XX_SET_ROW_ADDR 0x2B
#define ST77XX_WR_FRAME_MEM 0x2C
#define ST77XX_RD_FRAME_MEM 0x2E
#define ST77XX_SET_PARTIAL_DISP 0x30
#define ST77XX_SET_CLR_MODE 0x3A



void int_to_str(int num, char *str);
void st77xx_reset(void);
uint8_t st77xx_read_write(uint8_t data);
void st77xx_write_command(uint8_t cmd);
void st77xx_write_data(uint8_t data);
void st77xx_write_burst(uint8_t cmd, uint8_t* data, uint8_t len);
void st77xx_init(void);
void st77xx_fill_screen(uint16_t color);
void set_addr_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
//Not working yet
void set_rotation(uint8_t rotation);
void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void draw_line_width(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint8_t width);
void draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color, uint16_t line_width, uint16_t fill_color) ;
void draw_char(uint16_t x, uint16_t y, char c, uint16_t color);
void draw_text(uint16_t x, uint16_t y, const char *text, uint16_t color);
void draw_stylish_number(uint16_t x, uint16_t y, int number, uint16_t color, uint16_t shadow, uint8_t scale);
void draw_stylish_text(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t shadow, uint8_t scale);

#endif
