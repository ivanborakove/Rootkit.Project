#include "memory_utils.h"
#include "breakpoint_detector.h"
#include <linux/kallsyms.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

static int has_int3_in_range(unsigned char *addr, size_t len) {
    size_t i;
    for (i = 0; i < len; i++) {
        if (addr[i] == 0xCC)
            return 1;
    }
    return 0;
}

void check_for_breakpoints(void) {
    void *target_func = (void *)rk_resolve_symbol("fh_install_all");
    if (!target_func)
        return;

    if (has_int3_in_range(target_func, 128))
        printk(KERN_ALERT "KPRZ: Breakpoint (INT3) detected inside fh_install_all region!\n");
}
