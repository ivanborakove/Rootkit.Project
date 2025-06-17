#ifndef STRING_ENCRYPT_H
#define STRING_ENCRYPT_H

#include <linux/types.h>

struct encrypted_string {
    char *data;
    size_t len;
    int decrypted;
};

void decrypt_all_strings(void);
void encrypt_all_strings(void);

#endif
