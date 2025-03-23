
#ifndef LOG_H
#define LOG_H

#include <stdint.h>
void uart_send_string(const char *str);
void uart_send_char(char c);
void uart_send_int(int num);
void my_printf(const char *format, ...);
void uart_send_hex(uint32_t num);

#endif
