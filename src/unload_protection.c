#include "unload_protection.h"
#include <linux/module.h>

void lock_module_unloading(void) {
    THIS_MODULE->exit = NULL;
}
