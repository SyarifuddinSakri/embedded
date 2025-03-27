#include "ws.h"
#include "socket.h"
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "sha1.h"
#include "base64.h"

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
void handle_websocket_handshake(uint8_t socket_num, uint8_t *buffer) {
		if (getSn_IR(socket_num) & Sn_IR_CON) {
				setSn_IR(socket_num, Sn_IR_CON);  // Clear connection flag
		}
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
    else{
        buffer[1] = 126;
        buffer[2] = (payload_len >> 8) & 0xFF;  // Length MSB
        buffer[3] = payload_len & 0xFF;         // Length LSB
        frame_length += 3;
    } 
    
    // Copy payload data
    memcpy(buffer + frame_length, payload, payload_len);
    frame_length += payload_len;
    
    return frame_length;
}
void send_websocket_frame(uint8_t sn, uint8_t opcode, uint8_t *payload, uint16_t payload_len) 
{
    uint8_t ws_frame[128];  // Adjust size as needed
    uint16_t frame_len;
    
    frame_len = websocket_encode_frame(payload, payload_len, ws_frame, opcode);
    send(sn, ws_frame, frame_len);
}

// Usage examples:
void send_text(uint8_t sn, const char *text) {
    send_websocket_frame(sn, 0x1, (uint8_t *)text, strlen(text));
}

void send_binary(uint8_t sn, uint8_t *data, uint16_t len) {
    send_websocket_frame(sn, 0x2, data, len);
}

void send_ping(uint8_t sn) {
    send_websocket_frame(sn, 0x9, NULL, 0);  // Ping with no payload
}

void send_close(uint8_t sn, uint16_t status_code) {
    uint8_t close_frame[2];
    close_frame[0] = (status_code >> 8) & 0xFF;  // Status MSB
    close_frame[1] = status_code & 0xFF;         // Status LSB
    send_websocket_frame(sn, 0x8, close_frame, 2);
}

/**
 * @brief Parses WebSocket frame header
 * @param data Incoming data buffer
 * @param len Length of incoming data
 * @param frame Output frame structure
 * @return Number of bytes consumed by header
 */
uint16_t parse_websocket_header(uint8_t *data, uint16_t len, ws_frame_t *frame) {
    if (len < 2) return 0;  // Minimum header size
    
    frame->fin = (data[0] & 0x80) >> 7;
    frame->opcode = data[0] & 0x0F;
    frame->mask = (data[1] & 0x80) >> 7;
    frame->payload_len = data[1] & 0x7F;
    
    uint16_t header_len = 2;
    
    // Extended payload length
    if (frame->payload_len == 126) {
        if (len < 4) return 0;
        frame->payload_len = (data[2] << 8) | data[3];
        header_len += 2;
    } else if (frame->payload_len == 127) {
        if (len < 10) return 0;
        // For STM32F1 we'll only support 32-bit lengths
        frame->payload_len = ((uint64_t)data[2] << 56) | ((uint64_t)data[3] << 48) |
                           ((uint64_t)data[4] << 40) | ((uint64_t)data[5] << 32) |
                           ((uint64_t)data[6] << 24) | ((uint64_t)data[7] << 16) |
                           ((uint64_t)data[8] << 8) | data[9];
        header_len += 8;
    }
    
    // Masking key
    if (frame->mask) {
        if (len < header_len + 4) return 0;
        memcpy(frame->masking_key, data + header_len, 4);
        header_len += 4;
    }
    
    frame->payload_data = data + header_len;
    return header_len;
}

/**
 * @brief Unmasks WebSocket payload data
 * @param frame Frame structure with payload
 */
void unmask_websocket_payload(ws_frame_t *frame) {
    if (!frame->mask || !frame->payload_data) return;
    
    for (uint64_t i = 0; i < frame->payload_len; i++) {
        frame->payload_data[i] ^= frame->masking_key[i % 4];
    }
}

/**
 * @brief Handles incoming WebSocket frames
 * @param sn Socket number
 * @param data Received data
 * @param len Length of received data
 */
void handle_websocket_frame(uint8_t sn, uint8_t *data, uint16_t len) {
    ws_frame_t frame;
    uint16_t header_len = parse_websocket_header(data, len, &frame);
    
    if (header_len == 0 || header_len + frame.payload_len > len) {
        // Invalid frame
        send_close(sn, 1002);  // Protocol error
        return;
    }
    
    // Unmask client-to-server frames
    if (frame.mask) {
        unmask_websocket_payload(&frame);
    }
    
    // Handle by opcode
    switch (frame.opcode) {
        case 0x0:  // Continuation
            // Handle fragmented message (implementation depends on your state machine)
            break;
            
        case 0x1:  // Text
            handle_text_frame(sn, frame.payload_data, frame.payload_len);
            break;
            
        case 0x2:  // Binary
            handle_binary_frame(sn, frame.payload_data, frame.payload_len);
            break;
            
        case 0x8:  // Close
            handle_close_frame(sn, frame.payload_data, frame.payload_len);
            break;
            
        case 0x9:  // Ping
            handle_ping_frame(sn, frame.payload_data, frame.payload_len);
            break;
            
        case 0xA:  // Pong
            handle_pong_frame(sn, frame.payload_data, frame.payload_len);
            break;
            
        default:
            // Unknown opcode
            send_close(sn, 1003);  // Unsupported data
            break;
    }
}

// Example handler implementations
void handle_text_frame(uint8_t sn, uint8_t *data, uint16_t len) {
    // Echo back the text message
    send_text(sn, (char *)data);
    
    // Or process the text data as needed
    // data[len] = '\0';  // Null-terminate if needed
    // printf("Received text: %s\n", data);
}

void handle_binary_frame(uint8_t sn, uint8_t *data, uint16_t len) {
    // Process binary data
    // Example: send back first byte incremented
    if (len > 0) {
        uint8_t response = data[0] + 1;
        send_binary(sn, &response, 1);
    }
}

void handle_close_frame(uint8_t sn, uint8_t *data, uint16_t len) {
    uint16_t status_code = 1000;  // Normal closure
    if (len >= 2) {
        status_code = (data[0] << 8) | data[1];
    }
    
    // Echo back close frame
    send_close(sn, status_code);
    
    // Close the socket
    close(sn);
}

void handle_ping_frame(uint8_t sn, uint8_t *data, uint16_t len) {
    // Respond with Pong (may echo the same payload)
    send_pong(sn, data, len);
}

void handle_pong_frame(uint8_t sn, uint8_t *data, uint16_t len) {
    // Optional: track round-trip time for latency measurement
    // (No response needed for Pong frames)
}

void send_pong(uint8_t sn, uint8_t *data, uint16_t len) {
    if (len > 125) len = 125;  // Control frames can't be > 125 bytes
    send_websocket_frame(sn, 0xA, data, len);
}
