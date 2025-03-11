#include "sysout.h"
#include "libopencm3/stm32/gpio.h"
#include <libopencm3/stm32/usart.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/**
    * @brief Sends message via USART1.
* This is a custom function that send message through UART 
*
* @param message in char pointer
*/

void sysoutln(const char *format, ...) {
    char buffer[128];  // Adjust size as needed
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer) - 1, format, args); // -1 to ensure null termination
    buffer[sizeof(buffer) - 1] = '\0';  // Force null termination
    va_end(args);

    for (uint8_t i = 0; i < strlen(buffer); i++) {
        usart_send_blocking(USART1, buffer[i]);
    }
    usart_send_blocking(USART1, '\n');
}
void enable_sysout(void){
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                  GPIO_USART1_RX);

    usart_set_baudrate(USART1, 115200);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

    usart_enable(USART1);
}
