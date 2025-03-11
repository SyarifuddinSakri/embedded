#ifndef SYSOUT_H
#define SYSOUT_H

#include <stdint.h>
void sysout(char* message);
void sysout_hex(uint8_t data);
void sysout_str_hex(char* str, uint8_t data);

#endif
