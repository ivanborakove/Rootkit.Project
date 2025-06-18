#include "key_injector.h"
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/input/keyboard.h>
#include <linux/kallsyms.h>

static struct input_dev *input_dev;

void inject_keystroke(int keycode) {
    if (!input_dev)
        return;

    input_report_key(input_dev, keycode, 1);
    input_sync(input_dev);
    input_report_key(input_dev, keycode, 0);
    input_sync(input_dev);
}

static int __init key_injector_init(void) {
    input_dev = input_allocate_device();
    if (!input_dev)
        return -ENOMEM;

    input_dev->name = "kprz_virtual_keyboard";
    input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_SYN);

    __set_bit(KEY_A, input_dev->keybit);
    __set_bit(KEY_B, input_dev->keybit);
    __set_bit(KEY_C, input_dev->keybit);
    __set_bit(KEY_ENTER, input_dev->keybit);
    __set_bit(KEY_SPACE, input_dev->keybit);

    if (input_register_device(input_dev)) {
        input_free_device(input_dev);
        input_dev = NULL;
        return -EIO;
    }

    return 0;
}

static void __exit key_injector_exit(void) {
    if (input_dev) {
        input_unregister_device(input_dev);
        input_dev = NULL;
    }
}

module_init(key_injector_init);
module_exit(key_injector_exit);
