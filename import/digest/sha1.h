
#include <stdint.h>
typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    uint8_t buffer[64];
} SHA1_CTX;
void SHA1_Transform(uint32_t state[5], const uint8_t buffer[64]);
void SHA1_Init(SHA1_CTX *context);
void SHA1_Update(SHA1_CTX *context, const uint8_t *data, uint32_t len);
void SHA1_Final(SHA1_CTX *context, uint8_t digest[20]);
void sha1_hash(const uint8_t *data, uint32_t len, uint8_t hash[20]);
