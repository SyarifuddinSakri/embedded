#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "portmacro.h"
#include "projdefs.h"
#include "task.h"
#include "semphr.h"
#include "socket.h"
#include "log.h"
#include "sha.h"
#include "ws_server.h"

#define PORT 8000
#define BUFFER_SIZE 1024
#define SOCKET_NUMBER 1

extern SemaphoreHandle_t w5500_mutex;

uint8_t buffer[BUFFER_SIZE];
uint8_t socket_id = 5; // W5500 socket
static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
#define ROTLEFT(a, b) ((a << b) | (a >> (32 - b)))

void websocket_handshake(int sock) {
    char ws_key[64] = {0};
    char response[256] = {0};
    
    // Read HTTP request
    int len = recv(sock, buffer, BUFFER_SIZE - 1);
    if (len <= 0) return;

    // Find "Sec-WebSocket-Key"
    char *key_start = strstr((char *)buffer, "Sec-WebSocket-Key: ");
    if (!key_start) return;
    key_start += 19; // Move past "Sec-WebSocket-Key: "
    char *key_end = strstr(key_start, "\r\n");
    if (!key_end) return;
    int key_len = key_end - key_start;
    strncpy(ws_key, key_start, key_len);
    ws_key[key_len] = '\0';

    // Append GUID and compute SHA1 hash
    strcat(ws_key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    uint8_t sha1_result[20];
    sha1((uint8_t *)ws_key, strlen(ws_key), sha1_result);

    // Base64 encode SHA1 hash
    char accept_key[32] = {0};
    base64_encode(sha1_result, 20, accept_key);

    // Send WebSocket handshake response
    snprintf(response, sizeof(response),
             "HTTP/1.1 101 Switching Protocols\r\n"
             "Upgrade: websocket\r\n"
             "Connection: Upgrade\r\n"
             "Sec-WebSocket-Accept: %s\r\n\r\n", accept_key);

    send(sock, (uint8_t*)response, strlen(response)-1);
}

void websocket_loop(int sock) {
    while (1) {
        int len = recv(sock, buffer, BUFFER_SIZE - 1);
        if (len <= 0) break; // Disconnected

        // Handle WebSocket frames
        if ((buffer[0] & 0x0F) == 0x8) { // Close frame
            my_printf("WebSocket closing...\n");
            break;
        } else if ((buffer[0] & 0x0F) == 0x1) { // Text frame
            my_printf("Received: %s\n", buffer + 2); // Masked data

            // Send reply
            char reply[32] = {0x81, 11, 'H', 'e', 'l', 'l', 'o', ' ', 'B', 'r', 'o', '!'};
            send(sock, (uint8_t*)reply, 13-1);
        }
    }
    close(sock);
}

void websocket_server(void* args __attribute((unused))) {
    socket(socket_id, Sn_MR_TCP, PORT, 0);
    listen(socket_id);

    while (1) {
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

                // Read WS request
								websocket_handshake(SOCKET_NUMBER);
								websocket_loop(SOCKET_NUMBER);

                // Close connection after sending response
                disconnect(SOCKET_NUMBER);
                break;

            case SOCK_CLOSE_WAIT:
                disconnect(SOCKET_NUMBER);
                break;
			xSemaphoreGive(w5500_mutex);
			vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

}
void base64_encode(const unsigned char *input, int length, char *output) {
    int i, j;
    int pad = length % 3;
    int encoded_length = 4 * ((length + 2) / 3);

    for (i = 0, j = 0; i < length - pad; i += 3) {
        output[j++] = base64_table[(input[i] >> 2) & 0x3F];
        output[j++] = base64_table[((input[i] & 0x03) << 4) | ((input[i + 1] >> 4) & 0x0F)];
        output[j++] = base64_table[((input[i + 1] & 0x0F) << 2) | ((input[i + 2] >> 6) & 0x03)];
        output[j++] = base64_table[input[i + 2] & 0x3F];
    }

    if (pad) {
        output[j++] = base64_table[(input[i] >> 2) & 0x3F];
        if (pad == 1) {
            output[j++] = base64_table[(input[i] & 0x03) << 4];
            output[j++] = '=';
        } else {
            output[j++] = base64_table[((input[i] & 0x03) << 4) | ((input[i + 1] >> 4) & 0x0F)];
            output[j++] = base64_table[(input[i + 1] & 0x0F) << 2];
        }
        output[j++] = '=';
    }

    output[j] = '\0'; // Null-terminate the string
}
