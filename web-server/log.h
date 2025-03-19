
#ifndef LOG_H
#define LOG_H

void uart_send_string(const char *str);
void uart_send_char(char c);
void uart_send_int(int num);
void my_printf(const char *format, ...);

#endif
