#include <stdint.h>
#include <string.h>
#include "open_socket.h"
#include "FreeRTOS.h"
#include "task.h"
#include "projdefs.h"
#include "w5500.h"
#include "socket.h"
#include "log.h"
#include "ws.h"

#define SOCK_1  0
#define SOCK_2  1
#define SOCK_1_PORT 8000
#define SOCK_2_PORT 24
#define BUFF		1024

uint8_t rx1_buf[BUFF];
uint8_t tx1_buf[BUFF];
uint8_t rx2_buf[BUFF];
uint8_t tx2_buf[BUFF];

char* greeting1 = "This is greeting from WebSocket server\r\n";
char* greeting2 = "hello from port 24\r\n";
void open_socket1(void* args __attribute((unused))){
	for(;;){
		switch (getSn_SR(SOCK_1)) {
			case SOCK_CLOSED:
				socket(SOCK_1, Sn_MR_TCP, SOCK_1_PORT, 0);
				listen(SOCK_1);
			break;

			case SOCK_ESTABLISHED:
				//handle comunication protocol here
				recv(SOCK_1, rx1_buf, BUFF);
				my_printf("This is data from client :\r\n %s",rx1_buf);
				handle_websocket_handshake(SOCK_1, rx1_buf);
				send_text(SOCK_1, greeting1);
				recv(SOCK_1, rx1_buf, BUFF);

				disconnect(SOCK_1);
				close(SOCK_1);
			break;
			case SOCK_CLOSE_WAIT:
				disconnect(SOCK_1);
				close(SOCK_1);
			break;
		
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
