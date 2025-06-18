#include "delayed_activation.h"
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched/signal.h>

static struct task_struct *delayed_task;

static int delayed_fn(void *data) {
    msleep(60000);
    printk(KERN_INFO "KPRZ: Delayed activation triggered.\n");
    return 0;
}

void start_delayed_activation(void) {
    if (!delayed_task)
        delayed_task = kthread_run(delayed_fn, NULL, "kprz_delayed_task");
}
