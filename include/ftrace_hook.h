#ifndef FTRACE_HOOK_H
#define FTRACE_HOOK_H

#include <linux/ftrace.h>
#include <linux/ptrace.h>
#include <linux/version.h>
#include <linux/types.h>
#include "ftrace_regs.h"

struct fh_hook {
    const char *name;
    void *func;
    void *orig;
    unsigned long addr;
    struct ftrace_ops ops;
};

#define HOOK(n, f, o) { .name = (n), .func = (f), .orig = (o) }

#define FH_GET_IP(fregs) rk_ftrace_get_ip(fregs)
#define FH_SET_IP(fregs, ip) rk_ftrace_set_ip(fregs, ip)


int fh_install_hook(struct fh_hook *hook);
void fh_remove_hook(struct fh_hook *hook);
int fh_install_hooks(struct fh_hook *hooks, size_t count);
void fh_remove_hooks(struct fh_hook *hooks, size_t count);
int fh_install_all(void);
void fh_remove_all(void);
void fh_set_hooks(struct fh_hook *hooks, size_t count);

#endif
