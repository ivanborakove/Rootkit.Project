#include "key_injector.h"
#include <linux/module.h>
#include <linux/input.h>
#include <linux/slab.h>

static struct input_dev *virtual_keyboard;

void inject_key(unsigned int keycode) {
    if (!virtual_keyboard)
        return;

    input_report_key(virtual_keyboard, keycode, 1);  // Press
    input_sync(virtual_keyboard);

    input_report_key(virtual_keyboard, keycode, 0);  // Release
    input_sync(virtual_keyboard);

    printk(KERN_INFO "KPRZ: Injected keycode %u\n", keycode);
}

int setup_key_injector(void) {
    int ret;

    virtual_keyboard = input_allocate_device();
    if (!virtual_keyboard)
        return -ENOMEM;

    virtual_keyboard->name = "kprz_virtual_keyboard";
    virtual_keyboard->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_SYN);

    /* Habilitar todas as teclas (keycodes 1 a 255) */
    for (int i = 1; i < 256; i++)
        set_bit(i, virtual_keyboard->keybit);

    ret = input_register_device(virtual_keyboard);
    if (ret) {
        input_free_device(virtual_keyboard);
        virtual_keyboard = NULL;
        return ret;
    }

    printk(KERN_INFO "KPRZ: Virtual keyboard device registered.\n");
    return 0;
}

void cleanup_key_injector(void) {
    if (virtual_keyboard) {
        input_unregister_device(virtual_keyboard);
        virtual_keyboard = NULL;
        printk(KERN_INFO "KPRZ: Virtual keyboard device unregistered.\n");
    }
}
