#include "ftrace_regs.h"
#include "memory_utils.h"
#include <linux/version.h>

static ftrace_get_ip_t ftrace_get_ip_ptr = NULL;
static ftrace_set_ip_t ftrace_set_ip_ptr = NULL;

int init_ftrace_regs(void) {
    ftrace_get_ip_ptr = (ftrace_get_ip_t)rk_resolve_symbol("ftrace_get_regs_ip");
    ftrace_set_ip_ptr = (ftrace_set_ip_t)rk_resolve_symbol("ftrace_set_regs_ip");

    if (!ftrace_get_ip_ptr || !ftrace_set_ip_ptr)
        return -1;

    return 0;
}

void cleanup_ftrace_regs(void) {
    ftrace_get_ip_ptr = NULL;
    ftrace_set_ip_ptr = NULL;
}

unsigned long rk_ftrace_get_ip(struct pt_regs *regs) {
    if (ftrace_get_ip_ptr)
        return ftrace_get_ip_ptr(regs);
    return 0;
}

void rk_ftrace_set_ip(struct pt_regs *regs, unsigned long ip) {
    if (ftrace_set_ip_ptr)
        ftrace_set_ip_ptr(regs, ip);
}

EXPORT_SYMBOL_GPL(init_ftrace_regs);
EXPORT_SYMBOL_GPL(cleanup_ftrace_regs);
EXPORT_SYMBOL_GPL(rk_ftrace_get_ip);
EXPORT_SYMBOL_GPL(rk_ftrace_set_ip);
