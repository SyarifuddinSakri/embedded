
#include <libopencm3/stm32/usart.h>
#include "log.h"
void uart_send_string(const char *str) {
    while (*str) {
        usart_send_blocking(USART1, *str++);
    }
}
