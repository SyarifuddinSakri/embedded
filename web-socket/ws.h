#include <stdint.h>

typedef struct {
    uint8_t fin;
    uint8_t opcode;
    uint8_t mask;
    uint64_t payload_len;
    uint8_t masking_key[4];
    uint8_t *payload_data;
} ws_frame_t;

void handle_websocket_handshake(uint8_t socket_num, uint8_t *buffer);
uint16_t websocket_encode_frame(uint8_t *payload, uint16_t payload_len, uint8_t *buffer, uint8_t opcode);
void send_websocket_frame(uint8_t sn, uint8_t opcode, uint8_t *payload, uint16_t payload_len);
void send_binary(uint8_t sn, uint8_t *data, uint16_t len);
void send_ping(uint8_t sn);
void send_text(uint8_t sn, const char *text);
void send_pong(uint8_t sn, uint8_t *data, uint16_t len);
void send_close(uint8_t sn, uint16_t status_code);
uint16_t parse_websocket_header(uint8_t *data, uint16_t len, ws_frame_t *frame);
void unmask_websocket_payload(ws_frame_t *frame);
void handle_websocket_frame(uint8_t sn, uint8_t *data, uint16_t len);
void handle_text_frame(uint8_t sn, uint8_t *data, uint16_t len);
void handle_binary_frame(uint8_t sn, uint8_t *data, uint16_t len);
void handle_close_frame(uint8_t sn, uint8_t *data, uint16_t len);
void handle_ping_frame(uint8_t sn, uint8_t *data, uint16_t len);
void handle_pong_frame(uint8_t sn, uint8_t *data, uint16_t len);
