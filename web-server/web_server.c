#include "web_server.h"
#include "httpServer.h"
#include "FreeRTOS.h"
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/iwdg.h>
#include "task.h"
#include "socket.h"
#include "log.h"

#define PORT 80
#define BUFFER_SIZE 256
#define MAX_HTTPSOCK 6
#define index_page "<!DOCTYPE html>"\
  "<html>"\
    "<head>"\
      "<title>SY webserver</title>"\
      "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
    "</head>"\
    "<body>"\
      "<h1>Makan nasi bro</h1>"\
      "<p>Web server active</p>"\
    "</body>"\
  "</html>"

uint8_t rx_buffer[BUFFER_SIZE];
uint8_t tx_buffer[BUFFER_SIZE];
uint8_t socknumlist[MAX_HTTPSOCK] = {0,1,2,3,4,5};
uint8_t stat;

void http_server_task(void* args __attribute((unused))){
    httpServer_init(tx_buffer, rx_buffer, MAX_HTTPSOCK, socknumlist);
    reg_httpServer_cbfunc(mcu_reset, wdt_reset);
    reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)index_page);

    for(;;){
        for (uint8_t i = 0; i < sizeof(socknumlist)/sizeof(socknumlist[0]); i++) {
                if(getSn_SR(socknumlist[i]) != SOCK_CLOSED) {
                httpServer_run(socknumlist[i]);
                }else{
                socket(socknumlist[i], Sn_MR_TCP, PORT, 0);
                listen(socknumlist[i]);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void mcu_reset(void){
    scb_reset_system();
}

void wdt_reset(void){
    iwdg_reset();
}
