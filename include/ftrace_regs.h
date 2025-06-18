#ifndef FTRACE_REGS_H
#define FTRACE_REGS_H

#include <linux/ptrace.h>

typedef unsigned long (*ftrace_get_ip_t)(struct pt_regs *regs);
typedef void (*ftrace_set_ip_t)(struct pt_regs *regs, unsigned long ip);

int init_ftrace_regs(void);
void cleanup_ftrace_regs(void);
unsigned long rk_ftrace_get_ip(struct pt_regs *regs);
void rk_ftrace_set_ip(struct pt_regs *regs, unsigned long ip);

#endif
