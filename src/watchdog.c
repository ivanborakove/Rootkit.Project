#include "watchdog.h"
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched/signal.h>

static struct task_struct *watchdog_task;

static int watchdog_fn(void *data) {
    while (!kthread_should_stop()) {
        msleep(5000);
    }
    return 0;
}

void start_watchdog(void) {
    if (!watchdog_task)
        watchdog_task = kthread_run(watchdog_fn, NULL, "kprz_watchdog");
}

void stop_watchdog(void) {
    if (watchdog_task) {
        kthread_stop(watchdog_task);
        watchdog_task = NULL;
    }
}
