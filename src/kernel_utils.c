#include "memory_utils.h"
#include "kernel_utils.h"
#include <linux/kallsyms.h>
#include <asm/processor-flags.h>
#include <asm/paravirt.h>

void rk_disable_wp(void) {
    preempt_disable();
    barrier();
    write_cr0(read_cr0() & ~X86_CR0_WP);
    barrier();
}

void rk_enable_wp(void) {
    barrier();
    write_cr0(read_cr0() | X86_CR0_WP);
    barrier();
    preempt_enable();
}

unsigned long rk_resolve_symbol(const char *name) {
    return rk_resolve_symbol(name);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,7,0)
unsigned long rk_ftrace_get_ip(struct ftrace_regs *fregs) {
    return ftrace_get_regs_ip(fregs);
}

void rk_ftrace_set_ip(struct ftrace_regs *fregs, unsigned long ip) {
    ftrace_set_regs_ip(fregs, ip);
}
#endif
