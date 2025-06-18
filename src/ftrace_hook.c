#include "ftrace_hook.h"
#include "write_protection.h"
#include "memory_utils.h"
#include "ftrace_regs.h"
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,11,0)
#define FH_FTRACE_RECURSION_FLAG FTRACE_OPS_FL_RECURSION
#else
#define FH_FTRACE_RECURSION_FLAG FTRACE_OPS_FL_RECURSION_SAFE
#endif

struct fh_hook *fh_hooks = NULL;
size_t fh_hook_count = 0;

static int resolve_hook_address(struct fh_hook *hook) {
    hook->addr = rk_resolve_symbol(hook->name);
    if (!hook->addr)
        return -ENOENT;
    *((unsigned long *)hook->orig) = hook->addr;
    return 0;
}

static void notrace fh_thunk(unsigned long ip, unsigned long parent_ip,
                             struct ftrace_ops *ops, struct ftrace_regs *fregs) {
    struct fh_hook *hook = container_of(ops, struct fh_hook, ops);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,7,0)
    rk_ftrace_set_ip((struct pt_regs *)fregs, (unsigned long)hook->func);
#else
    struct pt_regs *regs = (struct pt_regs *)fregs;
    regs->ip = (unsigned long)hook->func;
#endif
}

int fh_install_hook(struct fh_hook *hook) {
    int err;

    err = resolve_hook_address(hook);
    if (err)
        return err;

    hook->ops.func = fh_thunk;
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS | FH_FTRACE_RECURSION_FLAG | FTRACE_OPS_FL_IPMODIFY;

    err = ftrace_set_filter_ip(&hook->ops, hook->addr, 0, 0);
    if (err)
        return err;

    err = register_ftrace_function(&hook->ops);
    if (err) {
        ftrace_set_filter_ip(&hook->ops, hook->addr, 1, 0);
        return err;
    }

    return 0;
}

void fh_remove_hook(struct fh_hook *hook) {
    unregister_ftrace_function(&hook->ops);
    ftrace_set_filter_ip(&hook->ops, hook->addr, 1, 0);
}

int fh_install_hooks(struct fh_hook *hooks, size_t count) {
    size_t i;
    int err;

    if (init_ftrace_regs() != 0)
        return -1;

    for (i = 0; i < count; i++) {
        err = fh_install_hook(&hooks[i]);
        if (err)
            return err;
    }

    return 0;
}

void fh_remove_hooks(struct fh_hook *hooks, size_t count) {
    size_t i;

    for (i = 0; i < count; i++)
        fh_remove_hook(&hooks[i]);

    cleanup_ftrace_regs();
}

/* ---------- IMPLEMENTAÇÃO DAS FUNÇÕES QUE ESTÃO FALTANDO ---------- */

int fh_install_all(void) {
    return fh_install_hooks(fh_hooks, fh_hook_count);
}

void fh_remove_all(void) {
    fh_remove_hooks(fh_hooks, fh_hook_count);
}

void fh_set_hooks(struct fh_hook *new_hooks, size_t count) {
    fh_hooks = new_hooks;
    fh_hook_count = count;
}

/* ------------------------------------------------------------------ */

EXPORT_SYMBOL_GPL(fh_install_hook);
EXPORT_SYMBOL_GPL(fh_remove_hook);
EXPORT_SYMBOL_GPL(fh_install_hooks);
EXPORT_SYMBOL_GPL(fh_remove_hooks);
EXPORT_SYMBOL_GPL(fh_install_all);
EXPORT_SYMBOL_GPL(fh_remove_all);
EXPORT_SYMBOL_GPL(fh_set_hooks);
