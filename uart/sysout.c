#include "sysout.h"
#include <libopencm3/stm32/usart.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
    * @brief Sends message via USART1.
* This is a custom function that send message through UART 
*
* @param message in char pointer
*/
const char hex_digits[] = "0123456789ABCDEF";
void sysout(char* message){
    for(uint8_t i=0; i<strlen(message); i++){
            usart_send_blocking(USART1, message[i]);
    }
    usart_send_blocking(USART1, '\n');

}

void sysout_hex(uint8_t data) {
    char hex_str[3];  // 2 hex digits + null terminator

    hex_str[0] = hex_digits[(data >> 4) & 0x0F]; // Upper nibble
    hex_str[1] = hex_digits[data & 0x0F];        // Lower nibble
    hex_str[2] = '\0'; // Null terminator

    sysout(hex_str);   // Use your existing sysout function
}

void sysout_str_hex(char* str, uint8_t data){
    uint8_t len = strlen(str);
    char* output = malloc(len+3);
    if(output == NULL){
        return;
    }
    strcpy(output, str);

    output[len] = hex_digits[(data>>4) & 0x0F];//Upper nibble
    output[len+1] = hex_digits[data & 0x0F];//Lower nibble
    output[len+2] = '\0';

    sysout(output);
    free(output);
}
