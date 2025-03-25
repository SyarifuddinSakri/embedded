#include <stdint.h>
static const char base64_table[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
int base64_encode(const uint8_t *data, uint32_t input_length, char *output);
