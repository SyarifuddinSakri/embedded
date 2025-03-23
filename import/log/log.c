
#include <libopencm3/stm32/usart.h>
#include <stdarg.h>
#include <stdint.h>
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

// New: Convert number to hex string
void uart_send_hex(uint32_t num) {
    const char hex_chars[] = "0123456789ABCDEF";
    char buffer[10];  // Buffer for hex conversion
    int i = 0;

    if (num == 0) {
        uart_send_string("0x0");
        return;
    }

    uart_send_string("0x");  // Print "0x" prefix

    while (num > 0) {
        buffer[i++] = hex_chars[num % 16];
        num /= 16;
    }

    while (i > 0) {
        uart_send_char(buffer[--i]);  // Print in reverse order
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
                case 'p':  // Pointer (Hex Address)
                    uart_send_hex((uintptr_t)va_arg(args, void *));
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
