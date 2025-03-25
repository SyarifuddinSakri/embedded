#include <stdint.h>
#include "base64.h"

int base64_encode(const uint8_t *data, uint32_t input_length, char *output) {
    uint32_t i = 0, j = 0;
    uint8_t octet_a, octet_b, octet_c;
    uint32_t triple;
    uint32_t output_length = 0;
    
    while (i < input_length) {
        octet_a = i < input_length ? data[i++] : 0;
        octet_b = i < input_length ? data[i++] : 0;
        octet_c = i < input_length ? data[i++] : 0;
        
        triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
        
        output[j++] = base64_table[(triple >> 3 * 6) & 0x3F];
        output[j++] = base64_table[(triple >> 2 * 6) & 0x3F];
        output[j++] = base64_table[(triple >> 1 * 6) & 0x3F];
        output[j++] = base64_table[(triple >> 0 * 6) & 0x3F];
        
        output_length += 4;
    }
    
    // Add padding if needed
    int padding = input_length % 3;
    if (padding) {
        output[output_length - 1] = '=';
        if (padding == 1) {
            output[output_length - 2] = '=';
        }
    }
    
    output[output_length] = '\0';
    return output_length;
}
