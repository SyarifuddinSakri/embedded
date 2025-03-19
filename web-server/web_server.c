#include "web_server.h"
#include "httpServer.h"
#include "socket.h"
#include "log.h"
#include "w5500.h"
#include <stdint.h>


#define HTTP_SOCKET 0
#define PORT 80
#define BUFFER_SIZE 2048

uint8_t rx_buffer[BUFFER_SIZE];
uint8_t tx_buffer[BUFFER_SIZE];
uint8_t stat;

void http_server_task(void* args __attribute((unused))){
    stat = socket(HTTP_SOCKET, Sn_MR_TCP, PORT, 0);
    my_printf("socket status %d",stat);
    stat = listen(HTTP_SOCKET);
    my_printf("listen status %d", stat);
    for(;;){
        if(stat!=SOCK_OK){
            uart_send_string("Creating socket failed\r\n");
        }
    }
}
