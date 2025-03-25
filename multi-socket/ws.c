#include "sha1.h"
#include "base64.h"
#include "ws.h"
#include "socket.h"
#include <stdio.h>
#include <string.h>
#include "log.h"

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
void handle_websocket_handshake(uint8_t socket_num, uint8_t *buffer) {
    char *key_start = strstr((char*)buffer, "Sec-WebSocket-Key: ") + 19;
    char *key_end = strstr(key_start, "\r\n");
    char key[64] = {0};
    strncpy(key, key_start, key_end - key_start);
    
    // Concatenate with GUID
    char combined[128];
    sprintf(combined, "%s%s", key, WS_GUID);
    
    // SHA-1 hash
    uint8_t sha1[20];
    sha1_hash((uint8_t*)combined, strlen(combined), sha1);
    
    // Base64 encode
    char accept_key[64];
    base64_encode(sha1, 20, accept_key);
    
    // Send response
    char response[512];
    sprintf(response, 
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: %s\r\n\r\n", 
            accept_key);
	my_printf("server response : \r\n %s",response);
    
    send(socket_num, (uint8_t*)response, strlen(response));
}
/**
 * @brief Encodes data into a WebSocket frame
 * @param payload The data to send (string in your case)
 * @param payload_len Length of the payload
 * @param buffer Output buffer for the WebSocket frame
 * @param opcode WebSocket opcode (0x1 for text frames)
 * @return Length of the encoded WebSocket frame
 */
uint16_t websocket_encode_frame(uint8_t *payload, uint16_t payload_len, uint8_t *buffer, uint8_t opcode) 
{
    uint16_t frame_length = 0;
    
    // FIN (1 bit) + RSV1-3 (0) + opcode (4 bits)
    buffer[0] = 0x80 | (opcode & 0x0F);  // FIN bit set + opcode
    frame_length++;
    
    // Mask bit (0 for server->client) + payload length
    if (payload_len <= 125) {
        buffer[1] = payload_len;
        frame_length++;
    } 
    else if (payload_len <= 65535) {
        buffer[1] = 126;
        buffer[2] = (payload_len >> 8) & 0xFF;  // Length MSB
        buffer[3] = payload_len & 0xFF;         // Length LSB
        frame_length += 3;
    } 
    else {
        // For payloads > 65535 (not implemented here for STM32F1)
        return 0;
    }
    
    // Copy payload data
    memcpy(buffer + frame_length, payload, payload_len);
    frame_length += payload_len;
    
    return frame_length;
}

void send_websocket_text(uint8_t sn, const char *message) 
{
    uint8_t ws_frame[128];  // Adjust size based on your needs
    uint16_t frame_len;
    
    // Encode the string into a WebSocket frame (opcode 0x1 = text frame)
    frame_len = websocket_encode_frame((uint8_t *)message, strlen(message), ws_frame, 0x1);
    
    // Send the frame using your send function
    send(sn, ws_frame, frame_len);
}
