#ifndef SYSOUT_H
#define SYSOUT_H

void sysoutln(const char *format, ...);
void enable_sysout(void);
void uart_putchar(char ch);

#endif
