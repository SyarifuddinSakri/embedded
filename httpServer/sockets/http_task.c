#include "http_task.h"
#include "httpUtil.h"
#include <stdint.h>
#include "socket.h"
#include "log.h"

#define BUFF_SIZE 1024
#define SOCK_1 0
#define SOCK_1_PORT 80
uint8_t tx_buf[BUFF_SIZE];
uint8_t rx_buf[BUFF_SIZE];
const char html_page[] =
"<!DOCTYPE html>\r\n"
"<html>\r\n"
"<head>\r\n"
"  <title>My Minimal Page</title>\r\n"
"</head>\r\n"
"<body>\r\n"
"  <h1>Hello, World!</h1>\r\n"
"  <p>This is a minimal HTML page.</p>\r\n"
"</body>\r\n"
"</html>\r\n";

void taskhttp(void){
	
	uint8_t socks = socket(SOCK_1, Sn_MR_TCP, SOCK_1_PORT, 0);
	reg_httpServer_webContent((uint8_t*)"index.html", (uint8_t*)html_page);
	
	for(;;){
		httpServer_init(tx_buf, rx_buf, 1, &socks);
		httpServer_run(SOCK_1);
	}
}
