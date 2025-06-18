#ifndef HMAC_AUTH_H
#define HMAC_AUTH_H

#include <linux/types.h>

#define HMAC_KEY "kprz_secret_key"
#define HMAC_KEY_LEN 14
#define HMAC_DIGEST_SIZE 32

bool validate_hmac(const unsigned char *data, size_t data_len, const unsigned char *received_hmac);
void compute_hmac(const char *key, const unsigned char *data, size_t len, unsigned char *output);

#endif
