#ifndef _ESP8266_H
#define _ESP8266_H

#include <stdint.h>

void esp_send_byte(uint8_t data);
void esp_send_command(const char *str);
int response_is_ok(const char *response);
void esp8266_connect(void);
void esp_read_response(char *buff, uint16_t max_len);
#endif
