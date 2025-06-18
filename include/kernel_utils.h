#ifndef KPRZ_KERNEL_UTILS_H
#define KPRZ_KERNEL_UTILS_H

#include <linux/version.h>
#include <linux/types.h>
#include <linux/ftrace.h>

void rk_disable_wp(void);
void rk_enable_wp(void);
unsigned long rk_resolve_symbol(const char *name);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,7,0)
unsigned long rk_ftrace_get_ip(struct ftrace_regs *fregs);
void rk_ftrace_set_ip(struct ftrace_regs *fregs, unsigned long ip);
#endif

#endif
