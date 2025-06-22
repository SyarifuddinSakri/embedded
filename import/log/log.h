
#ifndef LOG_H
#define LOG_H

#include <stdint.h>
void uart_send_string(const char *str);
void uart_send_char(char c);
void uart_send_int(int num);
void my_printf(const char *format, ...);
void uart_send_hex(uint32_t num);
char uart_read_char(void);
void uart_read_line(char* line_buf, uint32_t len);

#endif
