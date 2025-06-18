#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/ktime.h>
#include <linux/version.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include "ftrace_hook.h"

asmlinkage void (*real_touch_softlockup_watchdog)(void);

asmlinkage void hooked_touch_softlockup_watchdog(void) {
    return;
}

static struct fh_hook watchdog_hook = {
    .name = "touch_softlockup_watchdog",
    .func = hooked_touch_softlockup_watchdog,
    .orig = &real_touch_softlockup_watchdog,
};

void start_watchdog(void) {
    fh_install_hooks(&watchdog_hook, 1);
}

void stop_watchdog(void) {
    fh_remove_hooks(&watchdog_hook, 1);
}

EXPORT_SYMBOL_GPL(start_watchdog);
EXPORT_SYMBOL_GPL(stop_watchdog);
