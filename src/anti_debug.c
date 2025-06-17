#include "anti_debug.h"
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static int file_exists(const char *path) {
    struct file *f = filp_open(path, O_RDONLY, 0);
    if (IS_ERR(f))
        return 0;
    filp_close(f, NULL);
    return 1;
}

static int detect_vmware(void) {
    return file_exists("/sys/class/dmi/id/product_name");
}

static int detect_virtualbox(void) {
    return file_exists("/sys/module/vboxguest");
}

static int detect_qemu(void) {
    return file_exists("/sys/class/dmi/id/sys_vendor");
}

static int detect_debugging_tools(void) {
    return file_exists("/usr/bin/gdb") || file_exists("/usr/bin/strace") || file_exists("/usr/bin/ltrace");
}

void anti_debug_and_vm_checks(void) {
    if (detect_vmware())
        return;
    if (detect_virtualbox())
        return;
    if (detect_qemu())
        return;
    if (detect_debugging_tools())
        return;
}
