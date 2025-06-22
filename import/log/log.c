
#include <libopencm3/stm32/usart.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include "log.h"
#include "libopencm3/stm32/f1/usart.h"

void uart_send_char(char c) {
    usart_send_blocking(USART1, c);
}

char uart_read_char(void) {
    // Wait until data is received
    while (!(USART_SR(USART1) & USART_SR_RXNE));
    return usart_recv(USART1);
}


void uart_read_line(char* line_buf, uint32_t len){
	memset(line_buf, 0, len);
    uint32_t i = 0;
    while (1) {
        char c = uart_read_char();

        if (c == '\n' || c == '\r') {
            line_buf[i] = '\0'; // Null-terminate
            break;
        }

        if (i < len - 1) {
            line_buf[i++] = c;
        } else {
            // Optional: handle overflow
            break;
        }
    }
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
            int width = 0;

            // Check for zero-padding width like %02X
            if (*format == '0') {
                format++;
                if (*format >= '0' && *format <= '9') {
                    width = *format - '0';
                    format++;
                }
            }

            switch (*format) {
                case 'd':
                    uart_send_int(va_arg(args, int));
                    break;
                case 's':
                    uart_send_string(va_arg(args, char *));
                    break;
                case 'c':
                    uart_send_char((char)va_arg(args, int));
                    break;
                case 'p':
                    uart_send_hex((uintptr_t)va_arg(args, void *));
                    break;
                case 'x':
                case 'X': {
                    int val = va_arg(args, int);
                    char buf[9]; // Enough for 32-bit hex
                    int uppercase = (*format == 'X') ? 1 : 0;

                    // Convert to hex string
                    int i = 0;
                    do {
                        int digit = val & 0xF;
                        if (digit < 10)
                            buf[i++] = '0' + digit;
                        else
                            buf[i++] = (uppercase ? 'A' : 'a') + (digit - 10);
                        val >>= 4;
                    } while (val && i < 8);

                    // Zero padding if requested
                    while (i < width) buf[i++] = '0';

                    // Send hex string in reverse
                    while (i--)
                        uart_send_char(buf[i]);
                    break;
                }
                case '%':
                    uart_send_char('%');
                    break;
                default:
                    uart_send_char('?');
                    break;
            }
        } else {
						if(*format == '\n'){
							uart_send_char('\r');
						}
            uart_send_char(*format);
        }
        format++;
    }

    va_end(args);
}
