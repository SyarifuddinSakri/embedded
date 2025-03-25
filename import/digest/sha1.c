#include <string.h>
#include <stdint.h>
#include "sha1.h"

#define SHA1_ROTL(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))

void SHA1_Transform(uint32_t state[5], const uint8_t buffer[64]) {
    uint32_t a, b, c, d, e, temp;
    uint32_t w[80];
    uint32_t i;
    
    // Initialize registers with the previous intermediate values
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    
    // Convert 16 32-bit words from big-endian
    for (i = 0; i < 16; i++) {
        w[i] = ((uint32_t)buffer[i * 4 + 0] << 24) |
               ((uint32_t)buffer[i * 4 + 1] << 16) |
               ((uint32_t)buffer[i * 4 + 2] << 8)  |
               ((uint32_t)buffer[i * 4 + 3]);
    }
    
    // Extend to 80 words
    for (i = 16; i < 80; i++) {
        w[i] = SHA1_ROTL(1, w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]);
    }
    
    // Main rounds
    for (i = 0; i < 80; i++) {
        if (i < 20) {
            temp = SHA1_ROTL(5, a) + ((b & c) | ((~b) & d)) + e + w[i] + 0x5A827999;
        } else if (i < 40) {
            temp = SHA1_ROTL(5, a) + (b ^ c ^ d) + e + w[i] + 0x6ED9EBA1;
        } else if (i < 60) {
            temp = SHA1_ROTL(5, a) + ((b & c) | (b & d) | (c & d)) + e + w[i] + 0x8F1BBCDC;
        } else {
            temp = SHA1_ROTL(5, a) + (b ^ c ^ d) + e + w[i] + 0xCA62C1D6;
        }
        
        e = d;
        d = c;
        c = SHA1_ROTL(30, b);
        b = a;
        a = temp;
    }
    
    // Update state
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

void SHA1_Init(SHA1_CTX *context) {
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = 0;
    context->count[1] = 0;
}

void SHA1_Update(SHA1_CTX *context, const uint8_t *data, uint32_t len) {
    uint32_t i, j;
    
    j = (context->count[0] >> 3) & 63;
    if ((context->count[0] += len << 3) < (len << 3)) context->count[1]++;
    context->count[1] += (len >> 29);
    
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64 - j));
        SHA1_Transform(context->state, context->buffer);
        for (; i + 63 < len; i += 64) {
            SHA1_Transform(context->state, &data[i]);
        }
        j = 0;
    } else {
        i = 0;
    }
    
    memcpy(&context->buffer[j], &data[i], len - i);
}

void SHA1_Final(SHA1_CTX *context, uint8_t digest[20]) {
    uint32_t i;
    uint8_t finalcount[8];
    uint8_t c;
    
    for (i = 0; i < 8; i++) {
        finalcount[i] = (uint8_t)((context->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
    }
    
    c = 0x80;
    SHA1_Update(context, &c, 1);
    while ((context->count[0] & 504) != 448) {
        c = 0x00;
        SHA1_Update(context, &c, 1);
    }
    
    SHA1_Update(context, finalcount, 8);
    for (i = 0; i < 20; i++) {
        digest[i] = (uint8_t)((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
    }
    
    memset(context, 0, sizeof(*context));
}

void sha1_hash(const uint8_t *data, uint32_t len, uint8_t hash[20]) {
    SHA1_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, data, len);
    SHA1_Final(&ctx, hash);
}
