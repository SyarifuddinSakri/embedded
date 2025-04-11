#include <stdint.h>
#include "open_socket.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "projdefs.h"
#include "w5500.h"
#include "socket.h"
#include "web_server.h"
#include "ws.h"

#define SOCK_1  0
#define SOCK_2  1
#define SOCK_1_PORT 8000
#define SOCK_2_PORT 80
#define BUFF		1024

uint8_t rx1_buf[BUFF];
uint8_t tx1_buf[BUFF];
uint8_t rx2_buf[BUFF];
uint8_t tx2_buf[BUFF];

char* greeting1 = "This is greeting from WebSocket server\r\n";
char* greeting2 = "hello from port 24\r\n";
extern SemaphoreHandle_t mutex_w5500;
void socket1_handle(void* args __attribute((unused))){
	for(;;){
			//Opening socket1 if it is closed
			switch (getSn_SR(SOCK_1)) {
				case SOCK_CLOSED:
					socket(SOCK_1, Sn_MR_TCP, SOCK_1_PORT, 0);
					listen(SOCK_1);
				break;

			case SOCK_ESTABLISHED:
					handle_websocket_handshake(SOCK_1, rx1_buf);
					send_text(SOCK_1, greeting1);
					recv(SOCK_1, rx1_buf, BUFF);

					//Finally disconnect to empty the socket used for another client
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

void socket2_handle(void* args __attribute((unused))){
	for(;;){
			//Opening socket2 if it is closed
			switch (getSn_SR(SOCK_2)) {
				case SOCK_CLOSED:
					socket(SOCK_2, Sn_MR_TCP, SOCK_2_PORT, 0);
					listen(SOCK_2);
				break;

			case SOCK_ESTABLISHED:
					http_server_handle(SOCK_2);
					recv(SOCK_2, rx2_buf, BUFF);
					//Finally disconnect to empty the socket used for another client
					disconnect(SOCK_2);
					close(SOCK_2);
			break;

				case SOCK_CLOSE_WAIT:
					disconnect(SOCK_2);
					close(SOCK_2);
				break;
			}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
