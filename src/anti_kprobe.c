#include "memory_utils.h"
#include "anti_kprobe.h"
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/kernel.h>

static unsigned int simple_checksum(void *addr, size_t len) {
    unsigned char *p = addr;
    unsigned int sum = 0;
    size_t i;

    for (i = 0; i < len; i++)
        sum += p[i];

    return sum;
}

void check_function_integrity(void) {
    void *target = (void *)rk_resolve_symbol("fh_install_all");
    unsigned int original_sum;
    unsigned int current_sum;

    if (!target)
        return;

    original_sum = simple_checksum(target, 128);
    current_sum = simple_checksum(target, 128);

    if (original_sum != current_sum) {
        printk(KERN_ALERT "KPRZ: Anti-Kprobe Alert: fh_install_all integrity mismatch detected!\n");
    }
}
