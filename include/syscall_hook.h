#ifndef SYSCALL_HOOK_H
#define SYSCALL_HOOK_H

#include <linux/ptrace.h>

void setup_syscall_hook(void);
void remove_syscall_hook(void);

#endif
