#ifndef HEAP_ENCRYPT_H
#define HEAP_ENCRYPT_H

#include <linux/types.h>

void *allocate_protected_buffer(size_t size);
void free_protected_buffer(void *buf, size_t size);
void encrypt_buffer(void *buf, size_t size);
void decrypt_buffer(void *buf, size_t size);

#endif
