#include "heap_encrypt.h"
#include <linux/slab.h>
#include <linux/string.h>

#define XOR_KEY 0x5A

void *allocate_protected_buffer(size_t size) {
    void *buf = kmalloc(size, GFP_KERNEL);
    if (buf)
        memset(buf, 0, size);
    return buf;
}

void free_protected_buffer(void *buf, size_t size) {
    if (buf) {
        memset(buf, 0, size);
        kfree(buf);
    }
}

void encrypt_buffer(void *buf, size_t size) {
    size_t i;
    unsigned char *p = buf;
    for (i = 0; i < size; i++)
        p[i] ^= XOR_KEY;
}

void decrypt_buffer(void *buf, size_t size) {
    encrypt_buffer(buf, size);
}
