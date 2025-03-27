#include "web_server.h"
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/iwdg.h>
#include <stdint.h>
#include <string.h>
#include "socket.h"
#include "log.h"
#include "w5500.h"

#define BUFFER_SIZE 512

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
	"<script>console.log('makan nasi')</script>"
  "</body>"
  "</html>";

uint8_t rx_buffer[BUFFER_SIZE];

void http_server_handle(uint8_t sn) {
    int32_t data_size;

		if (getSn_IR(sn) & Sn_IR_CON) {
				setSn_IR(sn, Sn_IR_CON);  // Clear connection flag
		}

		// Read HTTP request
		data_size = recv(sn, rx_buffer, BUFFER_SIZE - 1);
		if (data_size > 0) {
				rx_buffer[data_size] = '\0';  // Null-terminate for safety
				my_printf("Received: %s\r\n", rx_buffer);

				// Simple request handling
				if (strncmp((char*)rx_buffer, "GET / ", 6) == 0 || strncmp((char*)rx_buffer, "GET /index.html", 15) == 0) {
						send(sn, (uint8_t*)http_index, sizeof(http_index) - 1);
				} else {
						send(sn, (uint8_t*)http_404, sizeof(http_404) - 1);
				}
		}

		// Close connection after sending response
		disconnect(sn);
}
