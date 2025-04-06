#include <libopencm3/stm32/i2c.h>
#include "ds3231.h"

#define DS3231_ADDR 0x68

// --- BCD helpers ---
static uint8_t bcd_to_dec(uint8_t val) {
    return (val >> 4) * 10 + (val & 0x0F);
}

static uint8_t dec_to_bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

// --- Write time to DS3231 ---
void ds3231_set_time(struct rtc_time *t) {
	uint8_t time_regs[7];
	time_regs[0] = dec_to_bcd(t->seconds);
	time_regs[1] = dec_to_bcd(t->minutes);
	time_regs[2] = dec_to_bcd(t->hours);
	time_regs[3] = dec_to_bcd(t->day);
	time_regs[4] = dec_to_bcd(t->date);
	time_regs[5] = dec_to_bcd(t->month);
	time_regs[6] = dec_to_bcd(t->year);

	uint8_t start_reg = 0x00;
	i2c_transfer7(I2C1, DS3231_ADDR, &start_reg, 1, NULL, 0); // set pointer
	i2c_transfer7(I2C1, DS3231_ADDR, time_regs, 7, NULL, 0);  // write values
}

// --- Read time from DS3231 ---
void ds3231_read_time(struct rtc_time *t) {
    uint8_t reg = 0x00;
    uint8_t buf[7];

    i2c_transfer7(I2C1, DS3231_ADDR, &reg, 1, buf, 7);

    t->seconds = bcd_to_dec(buf[0]);
    t->minutes = bcd_to_dec(buf[1]);
    t->hours   = bcd_to_dec(buf[2] & 0x3F); // mask 24hr
    t->day     = bcd_to_dec(buf[3]);
    t->date    = bcd_to_dec(buf[4]);
    t->month   = bcd_to_dec(buf[5] & 0x1F); // mask century bit
    t->year    = bcd_to_dec(buf[6]);
}

// --- Read temperature (in 0.25°C steps) ---
float ds3231_read_temperature(void) {
    uint8_t reg = 0x11; // temperature MSB register
    uint8_t buf[2];

    i2c_transfer7(I2C1, DS3231_ADDR, &reg, 1, buf, 2);

    int8_t msb = buf[0];
    uint8_t lsb = buf[1] >> 6; // only upper 2 bits valid
    float temp = msb + (lsb * 0.25f);
    return temp;
}
