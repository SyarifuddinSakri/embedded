#include <stddef.h>
#include <stdint.h>

#ifndef SHA_H
#define SHA_H
#define ROTLEFT(a, b) ((a << b) | (a >> (32 - b)))
typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    uint32_t state[5];
} SHA1_CTX;

void sha1_transform(SHA1_CTX *ctx, const uint8_t data[]);
void sha1_init(SHA1_CTX *ctx);
void sha1_update(SHA1_CTX *ctx, const uint8_t data[], size_t len);
void sha1_final(SHA1_CTX *ctx, uint8_t hash[]);
void sha1(const uint8_t *data, size_t len, uint8_t hash[20]);
#endif
