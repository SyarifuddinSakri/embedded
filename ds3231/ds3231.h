#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>

struct rtc_time {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;   // 1 = Sunday, 7 = Saturday
    uint8_t date;  // Day of month: 1–31
    uint8_t month; // 1–12
    uint8_t year;  // 0–99 (assume 2000 + year)
};

void ds3231_read_time(struct rtc_time *t);
void ds3231_set_time(struct rtc_time *t);
float ds3231_read_temperature(void);

#endif
