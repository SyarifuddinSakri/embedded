
#ifndef WS_SERVER_H
#define WS_SERVER_H
void websocket_server(void* args __attribute((unused)));
void base64_encode(const unsigned char *input, int length, char *output);
void websocket_loop(int sock);
void websocket_handshake(int sock);
#endif
