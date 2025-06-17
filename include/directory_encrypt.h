#ifndef DIRECTORY_ENCRYPT_H
#define DIRECTORY_ENCRYPT_H

#include <linux/types.h>
#include <linux/ptrace.h>

asmlinkage long hooked_getdents64(const struct pt_regs *regs);
extern asmlinkage long (*real_getdents64)(const struct pt_regs *);

#endif
