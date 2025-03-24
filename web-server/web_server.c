
#include "web_server.h"
#include "FreeRTOS.h"
#include <stdint.h>
#include <string.h>
#include "portmacro.h"
#include "task.h"
#include "semphr.h"
#include "socket.h"
#include "log.h"
#include "w5500.h"

#define PORT 80
#define BUFFER_SIZE 512
#define SOCKET_NUMBER 0

extern SemaphoreHandle_t w5500_mutex;

const char http_404[] =
  "HTTP/1.1 404 Not Found\r\n"
  "Content-Type: text/html\r\n"
  "Connection: close\r\n"
  "\r\n"
  "<html><body><h1>404 Not Found</h1></body></html>";

const char http_index[] =
  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/html\r\n"
  "Connection: close\r\n"
  "\r\n"
  "<!DOCTYPE html>"
  "<html>"
  "<head><title>Henlooo Uinaaaa</title></head>"
  "<body>"
  "<h1>Makan nasi bro</h1>"
  "<p>This is a simple embedded web server.</p>"
  "<script>"
  "  let ws = new WebSocket('ws://192.168.132.3:8000');" // Connect to WebSocket server
  "  ws.onopen = function() { console.log('WebSocket Connected!'); ws.send('Hello from browser'); };" // Send a message when connected
  "  ws.onmessage = function(event) { console.log('Received:', event.data); };" // Log incoming messages
  "  ws.onclose = function() { console.log('WebSocket Disconnected'); };" // Handle disconnection
  "</script>"

  "</body>"
  "</html>";
uint8_t rx_buffer[BUFFER_SIZE];

void http_server_task(void* args __attribute((unused))) {
    int32_t data_size;

    for (;;) {
		xSemaphoreTake(w5500_mutex, portMAX_DELAY);
        switch (getSn_SR(SOCKET_NUMBER)) {
            case SOCK_CLOSED:
                socket(SOCKET_NUMBER, Sn_MR_TCP, PORT, 0);
                listen(SOCKET_NUMBER);
                break;

            case SOCK_ESTABLISHED:
                if (getSn_IR(SOCKET_NUMBER) & Sn_IR_CON) {
                    setSn_IR(SOCKET_NUMBER, Sn_IR_CON);  // Clear connection flag
                }

                // Read HTTP request
                data_size = recv(SOCKET_NUMBER, rx_buffer, BUFFER_SIZE - 1);
                if (data_size > 0) {
                    rx_buffer[data_size] = '\0';  // Null-terminate for safety
                    my_printf("Received: %s\r\n", rx_buffer);

                    // Simple request handling
                    if (strncmp((char*)rx_buffer, "GET / ", 6) == 0 || strncmp((char*)rx_buffer, "GET /index.html", 15) == 0) {
                        send(SOCKET_NUMBER, (uint8_t*)http_index, sizeof(http_index) - 1);
                    } else {
                        send(SOCKET_NUMBER, (uint8_t*)http_404, sizeof(http_404) - 1);
                    }
                }

                // Close connection after sending response
                disconnect(SOCKET_NUMBER);
                break;

            case SOCK_CLOSE_WAIT:
                disconnect(SOCKET_NUMBER);
                break;
        }

		xSemaphoreGive(w5500_mutex);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
