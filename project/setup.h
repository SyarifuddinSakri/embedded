#include <stdint.h>

void clock_init(void);
void spi2_init(void);
void st7735_send_command(uint8_t cmd);
void st7735_send_data(uint8_t data);
void st7735_reset(void);
void st7735_init(void);
