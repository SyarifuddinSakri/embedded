#ifndef WEB_SERVER_H
#define WEB_SERVER_H

void http_server_task(void* args __attribute((unused)));
void mcu_reset(void);
void wdt_reset(void);

#endif
