#ifndef _ESP8266_H
#define _ESP8266_H

#include <stdint.h>

void esp_send_byte(uint8_t data);
void esp_send_string(const char *str);
uint8_t esp_read_byte(void);
void esp_read_string(char** buff);
#endif
