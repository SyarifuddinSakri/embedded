#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <stdint.h>
#include <stdlib.h>
#include "st77xx.h"
#include "blink.h"
#include "font1.h"

void st77xx_reset(void) {
    gpio_clear(GPIOB, ST77XX_RST);
    delay(100);
    gpio_set(GPIOB, ST77XX_RST);
    delay(100);
}
uint8_t st77xx_read_write(uint8_t data){
    //Wait for the SPI 2 Status Register Transmit is empty
    while((SPI_SR(SPI2)&SPI_SR_TXE)!= SPI_SR_TXE){
    }

    // Check for SPI errors
    if (SPI_SR(SPI2) & SPI_SR_OVR) {
        /*sysout("SPI Overrun Error");*/
        return 0xFF;
    }
    if (SPI_SR(SPI2) & SPI_SR_MODF) {
        /*sysout("SPI Mode Fault");*/
        return 0xFF;
    }

    SPI2_DR=data;
    /*spi_send(SPI2, data);*/

    //Wait until data returned from the peripheral
    while(!(SPI_SR(SPI2)&SPI_SR_RXNE)){
    }
    return SPI2_DR;
}

void st77xx_write_command(uint8_t cmd) {
    gpio_clear(GPIOB, ST77XX_DC);  // Command mode
    gpio_clear(GPIOB, ST77XX_CS);  // Select display
    st77xx_read_write(cmd);
    gpio_set(GPIOB, ST77XX_CS);    // Deselect display
}

void st77xx_write_data(uint8_t data) {
    gpio_set(GPIOB, ST77XX_DC);  // Data mode
    gpio_clear(GPIOB, ST77XX_CS);
    st77xx_read_write(data);
    gpio_set(GPIOB, ST77XX_CS);
}

void st77xx_write_burst(uint8_t cmd, uint8_t* data, uint8_t len){
    gpio_clear(GPIOB, ST77XX_DC);//Switch to command mode
    gpio_clear(GPIOB, ST77XX_CS);//Chip select
    st77xx_read_write(cmd);//Write command
    gpio_set(GPIOB, ST77XX_DC);//Switch to data mode
    for (int i=0; i<len; i++) {
        st77xx_read_write(data[i]);//Send the data
    }
    gpio_set(GPIOB, ST77XX_CS);
}

void st77xx_init(void) {
    st77xx_reset();
    
    st77xx_write_command(0x01);  // Software reset
    delay(10000);

    st77xx_write_command(0x11);  // Sleep Out
    delay(1000);

    st77xx_write_burst(0x3A, (uint8_t[]){0x05}, 1); //Defines color mode
    
    st77xx_write_burst(0x36, (uint8_t[]){0xC0}, 1); //memory data acces control 0x00(normal)

    st77xx_write_command(0x13);//Normal display mode on

    st77xx_write_command(0x29);  // Display On
}

void set_addr_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
    uint8_t x_val[] = {(x >> 8), (x & 0xFF), ((x + w -1)>>8), ((x + w -1) * 0xFF)};
    st77xx_write_burst(ST77XX_SET_COL_ADDR, x_val, 4);

    uint8_t y_val[] = {(y >> 8),(y & 0xFF),((y + h - 1) >> 8),((y + h - 1) & 0xFF)};
    st77xx_write_burst(ST77XX_SET_ROW_ADDR, y_val, 4);
}

void draw_pixel(uint16_t x, uint16_t y, uint16_t color){
    if (x >= ST77XX_WIDTH || y >= ST77XX_HEIGHT) return;

    set_addr_window(x, y, 1, 1);
    st77xx_write_command(0x2C);
    st77xx_write_data(color >> 8);
    st77xx_write_data(color & 0xFF);
}

// Fill the screen with a single color
void st77xx_fill_screen(uint16_t color) {
    set_addr_window(0, 0, ST77XX_WIDTH, ST77XX_HEIGHT);
    st77xx_write_command(ST77XX_WR_FRAME_MEM);

    for (uint32_t i = 0; i < (ST77XX_WIDTH * ST77XX_HEIGHT); i++) {
        st77xx_write_data(color >> 8);
        st77xx_write_data(color & 0xFF);
    }
}

void draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color){
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;
    
    while (1) {
        draw_pixel(x0, y0, color);  // Draw pixel at (x0, y0)

        if (x0 == x1 && y0 == y1) break;  // Line complete

        int16_t e2 = err * 2;
        if (e2 > -dy) {  // Move in x-direction
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {  // Move in y-direction
            err += dx;
            y0 += sy;
        }
    }

}
void draw_line_width(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint8_t width) {
    int16_t dx = abs(x1 - x0), sx = (x0 < x1) ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx + dy, e2;

    int8_t half_width = width / 2;

    while (1) {
        // Draw a thicker line by filling perpendicular pixels
        for (int8_t i = -half_width; i <= half_width; i++) {
            if (dx > dy) {
                draw_pixel(x0, y0 + i, color); // Thickening in Y direction
            } else {
                draw_pixel(x0 + i, y0, color); // Thickening in X direction
            }
        }

        if (x0 == x1 && y0 == y1) break;
        
        e2 = err * 2;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}


void draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color, uint16_t line_width, uint16_t fill_color) {
    int16_t x, y, p;

    // Draw filled circle if fill_color is set
    if (fill_color != 0xFFFF) {
        for (y = -r; y <= r; y++) {
            for (x = -r; x <= r; x++) {
                if ((x * x + y * y) <= (r * r)) {  // Inside the circle
                    draw_pixel(x0 + x, y0 + y, fill_color);
                }
            }
        }
    }

    // Draw thick circle outline correctly
    for (uint16_t w = 0; w < line_width; w++) {
        int16_t radius = r - w;
        if (radius < 0) break;  // Avoid negative radius

        x = radius;
        y = 0;
        p = 1 - radius;

        while (x >= y) {
            draw_pixel(x0 + x, y0 + y, color);
            draw_pixel(x0 - x, y0 + y, color);
            draw_pixel(x0 + x, y0 - y, color);
            draw_pixel(x0 - x, y0 - y, color);
            draw_pixel(x0 + y, y0 + x, color);
            draw_pixel(x0 - y, y0 + x, color);
            draw_pixel(x0 + y, y0 - x, color);
            draw_pixel(x0 - y, y0 - x, color);
            // Fill in additional width around the outline
            for (int16_t i = 1; i < line_width; i++) {
                draw_pixel(x0 + x - i, y0 + y, color);
                draw_pixel(x0 - x + i, y0 + y, color);
                draw_pixel(x0 + x - i, y0 - y, color);
                draw_pixel(x0 - x + i, y0 - y, color);
                draw_pixel(x0 + y, y0 + x - i, color);
                draw_pixel(x0 - y, y0 + x - i, color);
                draw_pixel(x0 + y, y0 - x + i, color);
                draw_pixel(x0 - y, y0 - x + i, color);
            }

            y++;
            if (p <= 0) {
                p += 2 * y + 1;
            } else {
                x--;
                p += 2 * (y - x) + 1;
            }
        }
    }
}

void draw_char(uint16_t x, uint16_t y, char c, uint16_t color) {
    if (c < 32 || c > 127) return;  // Skip unsupported characters

    const uint8_t *bitmap = font5x7[c - 32];

    for (int col = 0; col < 5; col++) {
        uint8_t line = bitmap[col]; // Get column bitmap

        for (int row = 0; row < 7; row++) {
            if (line & (1 << row)) {
                draw_pixel(x + col, y + row, color); // Foreground color
            } else {
            }
        }
    }
}

void draw_text(uint16_t x, uint16_t y, const char *text, uint16_t color) {
    while (*text) {
        draw_char(x, y, *text, color);
        x += 6;  // Move right (5 pixels + 1 spacing)
        text++;
    }
}

void set_rotation(uint8_t rotation) {
    uint8_t madctl = 0;

    switch (rotation) {
        case 0:   // 0 degrees (default)
            madctl = 0x00;
            break;
        case 1:   // 90 degrees
            madctl = 0x60;
            break;
        case 2:   // 180 degrees
            madctl = 0xC0;
            break;
        case 3:   // 270 degrees
            madctl = 0xA0;
            break;
        default:  // Invalid rotation, do nothing
            return;
    }

    st77xx_write_burst(0x36, (uint8_t[]){madctl}, 1);
}


void int_to_str(int num, char *str) {
    char temp[12]; // Max 10 digits + sign + null terminator
    int i = 0, j, isNegative = 0;

    // Handle negative numbers
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    // Extract digits in reverse order
    do {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);

    if (isNegative) temp[i++] = '-';

    // Reverse the string
    for (j = 0; j < i; j++) {
        str[j] = temp[i - j - 1];
    }
    str[i] = '\0'; // Null-terminate
}
void draw_stylish_number(uint16_t x, uint16_t y, int number, uint16_t color, uint16_t shadow, uint8_t scale) {
    char numStr[12];  // Buffer to store number as a string
    int_to_str(number, numStr);  // Convert number to string

    char *ptr = numStr;
    while (*ptr) {
        char digit = *ptr++;

        if (digit < '0' || digit > '9') continue;  // Ensure valid digit

        const uint8_t *bitmap = font5x7[digit - '0' + 16]; // Get bitmap for digit

        for (int col = 0; col < 5; col++) {
            uint8_t line = bitmap[col];

            for (int row = 0; row < 7; row++) {
                if (line & (1 << row)) {
                    // Draw shadow
                    for (int sx = 0; sx < scale; sx++) {
                        for (int sy = 0; sy < scale; sy++) {
                            draw_pixel(x + col * scale + sx + 1, y + row * scale + sy + 1, shadow);
                        }
                    }
                    // Draw main number
                    for (int sx = 0; sx < scale; sx++) {
                        for (int sy = 0; sy < scale; sy++) {
                            draw_pixel(x + col * scale + sx, y + row * scale + sy, color);
                        }
                    }
                }
            }
        }
        x += 6 * scale; // Move to next digit
    }
}
void draw_stylish_text(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t shadow, uint8_t scale) {
    while (*text) {
        char c = *text++;

        if (c < ' ' || c > '~') continue; // Ignore unsupported characters

        const uint8_t *bitmap = font5x7[c - ' ']; // Get character bitmap

        for (int col = 0; col < 5; col++) {
            uint8_t line = bitmap[col]; // Get column bitmap

            for (int row = 0; row < 7; row++) {
                if (line & (1 << row)) {
                    // Draw shadow (1-pixel diagonal offset)
                    for (int sx = 0; sx < scale; sx++) {
                        for (int sy = 0; sy < scale; sy++) {
                            draw_pixel(x + col * scale + sx + 1, y + row * scale + sy + 1, shadow);
                        }
                    }
                    // Draw main character
                    for (int sx = 0; sx < scale; sx++) {
                        for (int sy = 0; sy < scale; sy++) {
                            draw_pixel(x + col * scale + sx, y + row * scale + sy, color);
                        }
                    }
                }
            }
        }
        x += 6 * scale; // Move to next character (5px wide + 1px spacing)
    }
}

