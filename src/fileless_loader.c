#include "fileless_loader.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

static void *allocated_memory = NULL;

void setup_fileless_loader(void) {
    allocated_memory = vmalloc(4096);
    if (allocated_memory)
        memset(allocated_memory, 0x90, 4096);
}

void cleanup_fileless_loader(void) {
    if (allocated_memory)
        vfree(allocated_memory);
}
