#include "string_encrypt.h"
#include <linux/slab.h>
#include <linux/string.h>

#define XOR_KEY 0xAA

static struct encrypted_string *encrypted_strings = NULL;
static size_t string_count = 0;

static void xor_encrypt(char *data, size_t len) {
    size_t i;
    for (i = 0; i < len; i++)
        data[i] ^= XOR_KEY;
}

void register_encrypted_strings(struct encrypted_string *strings, size_t count) {
    encrypted_strings = strings;
    string_count = count;
}

void decrypt_all_strings(void) {
    size_t i;
    for (i = 0; i < string_count; i++) {
        if (!encrypted_strings[i].decrypted) {
            xor_encrypt(encrypted_strings[i].data, encrypted_strings[i].len);
            encrypted_strings[i].decrypted = 1;
        }
    }
}

void encrypt_all_strings(void) {
    size_t i;
    for (i = 0; i < string_count; i++) {
        if (encrypted_strings[i].decrypted) {
            xor_encrypt(encrypted_strings[i].data, encrypted_strings[i].len);
            encrypted_strings[i].decrypted = 0;
        }
    }
}
