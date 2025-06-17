#ifndef FTRACE_HOOK_H
#define FTRACE_HOOK_H

#include <linux/ftrace.h>

#define HOOK(_name, _function, _original) \
    {                                     \
        .name = (_name),                  \
        .function = (_function),          \
        .original = (_original),          \
    }

struct ftrace_hook {
    const char *name;
    void *function;
    void *original;
    unsigned long address;
    struct ftrace_ops ops;
};

int fh_install_hooks(void);
void fh_remove_hooks(void);
int fh_install_hook(struct ftrace_hook *hook);
void fh_remove_hook(struct ftrace_hook *hook);

#endif
