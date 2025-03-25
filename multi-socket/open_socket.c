#include <stdint.h>
#include <string.h>
#include "open_socket.h"
#include "FreeRTOS.h"
#include "task.h"
#include "projdefs.h"
#include "w5500.h"
#include "socket.h"

#define SOCK_1  0
#define SOCK_2  1

char* greeting1 = "hello from port 23\r\n";
char* greeting2 = "hello from port 24\r\n";
void open_socket1(void* args __attribute((unused))){
	for(;;){
		switch (getSn_SR(0)) {
			case SOCK_CLOSED:
				socket(0, Sn_MR_TCP, 23, 0);
				listen(0);
			break;

			case SOCK_ESTABLISHED:
				if(getSn_IR(0) & Sn_IR_CON){
					setSn_IR(0, Sn_IR_CON); // clear connection flag
				}

				send(0, (uint8_t*)greeting1, strlen(greeting1)-1);

				recv(0, (uint8_t*)greeting1, strlen(greeting1)-1);

				disconnect(0);
			break;
			case SOCK_CLOSE_WAIT:
				disconnect(0);
			break;
		
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

/*void open_socket2(void* args __attribute((unused))){*/
/*	for(;;){*/
/*		switch (getSn_SR(SOCK_2)) {*/
/*			case SOCK_CLOSED:*/
/*				socket(SOCK_2, Sn_MR_TCP, 24, 0);*/
/*				listen(SOCK_2);*/
/*			break;*/
/**/
/*			case SOCK_ESTABLISHED:*/
/*				if(getSn_IR(SOCK_2) & Sn_IR_CON){*/
/*					setSn_IR(SOCK_2, Sn_IR_CON); // clear connection flag*/
/*				}*/
/**/
/*				send(SOCK_2, (uint8_t*)greeting1, strlen(greeting1)-1);*/
/**/
/*				disconnect(SOCK_2);*/
/*			break;*/
/*			case SOCK_CLOSE_WAIT:*/
/*				disconnect(SOCK_2);*/
/*			break;*/
/**/
/*		}*/
/*		vTaskDelay(pdMS_TO_TICKS(100));*/
/*	}*/
/*}*/
