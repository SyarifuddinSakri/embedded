#include "esp8266.h"
#include "libopencm3/stm32/usart.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>

void esp_send_byte(uint8_t data) {
    usart_send_blocking(USART2, data);
}
void esp_send_string(const char *str) {
    while (*str) {
        esp_send_byte(*str++);
    }
}

uint8_t esp_read_byte(void){
	return usart_recv_blocking(USART2);
}

void esp_read_string(char** buff){
	uint8_t len = 1;
	while(1){

	*buff = realloc(*buff, len);

		if(*buff==NULL){
			my_printf("failed to allocate read string\r\n");
			free(*buff);
			break;
		}

		char b = esp_read_byte();
		if(b=='\n'|| b=='\r'){
		(*buff)[len-1] = '\0';
			break;
		}

		(*buff)[len-1] = b;
		len++;
	}
}
