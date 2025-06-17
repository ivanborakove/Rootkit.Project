#include "string_encrypt.h"
#include <linux/kernel.h>
#include <linux/module.h>

extern struct encrypted_string __start_string_encrypt_section[];
extern struct encrypted_string __stop_string_encrypt_section[];

static void xor_encrypt(char *data, size_t len) {
    size_t i;
    for (i = 0; i < len; i++)
        data[i] ^= 0xAA;
}

void decrypt_all_strings(void) {
    struct encrypted_string *str;
    for (str = __start_string_encrypt_section; str < __stop_string_encrypt_section; str++) {
        if (!str->decrypted) {
            xor_encrypt(str->data, str->len);
            str->decrypted = 1;
        }
    }
}

void encrypt_all_strings(void) {
    struct encrypted_string *str;
    for (str = __start_string_encrypt_section; str < __stop_string_encrypt_section; str++) {
        if (str->decrypted) {
            xor_encrypt(str->data, str->len);
            str->decrypted = 0;
        }
    }
}
