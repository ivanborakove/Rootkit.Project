#ifndef INLINE_HOOK_H
#define INLINE_HOOK_H

#include <linux/types.h>

struct inline_hook {
    void *target_func;
    void *hook_func;
    unsigned char original_bytes[12];
    size_t length;
};

int install_inline_hook(struct inline_hook *hook);
void remove_inline_hook(struct inline_hook *hook);
void setup_inline_hooks(void);
void remove_inline_hooks(void);

#endif
