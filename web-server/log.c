
#include <libopencm3/stm32/usart.h>
#include <stdarg.h>
#include "log.h"

void uart_send_char(char c) {
    usart_send_blocking(USART1, c);
}

void uart_send_string(const char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

void uart_send_int(int num) {
    char buffer[10];  // Buffer to hold the number as a string
    int i = 0;
    if (num == 0) {
        uart_send_char('0');
        return;
    }
    if (num < 0) {
        uart_send_char('-');
        num = -num;
    }
    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }
    while (i > 0) {
        uart_send_char(buffer[--i]);  // Print digits in reverse order
    }
}

void my_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    while (*format) {
        if (*format == '%') {
            format++;  // Move to format specifier
            switch (*format) {
                case 'd':  // Integer
                    uart_send_int(va_arg(args, int));
                    break;
                case 's':  // String
                    uart_send_string(va_arg(args, char *));
                    break;
                case 'c':  // Character
                    uart_send_char((char)va_arg(args, int));
                    break;
                case '%':  // Literal '%'
                    uart_send_char('%');
                    break;
                default:
                    uart_send_char('?');  // Unknown format
                    break;
            }
        } else {
            uart_send_char(*format);
        }
        format++;
    }
    
    va_end(args);
}
