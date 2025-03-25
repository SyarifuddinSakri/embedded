#include <stdint.h>

void handle_websocket_handshake(uint8_t socket_num, uint8_t *buffer);
void send_websocket_text(uint8_t sn, const char *message);
uint16_t websocket_encode_frame(uint8_t *payload, uint16_t payload_len, uint8_t *buffer, uint8_t opcode);
