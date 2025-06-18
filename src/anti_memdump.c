#include "memory_utils.h"
#include "anti_memdump.h"
#include "kernel_utils.h"
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/kallsyms.h>
#include <linux/vmalloc.h>

static unsigned long start_addr;
static unsigned long end_addr;

void setup_anti_memdump(void) {
    unsigned long module_base;
    unsigned long module_size;
    unsigned long (*get_module_base)(struct module *);
    unsigned long (*get_module_size)(struct module *);

    get_module_base = (void *)rk_resolve_symbol("module_core");
    get_module_size = (void *)rk_resolve_symbol("module_core_size");

    if (!get_module_base || !get_module_size)
        return;

    module_base = get_module_base(THIS_MODULE);
    module_size = get_module_size(THIS_MODULE);

    start_addr = module_base;
    end_addr = module_base + module_size;

    ((int (*)(unsigned long, int))rk_resolve_symbol("set_memory_ro"))(start_addr, (end_addr - start_addr) / PAGE_SIZE);
    ((int (*)(unsigned long, int))rk_resolve_symbol("set_memory_nx"))(start_addr, (end_addr - start_addr) / PAGE_SIZE);

    printk(KERN_INFO "KPRZ: Anti-MemDump enabled on 0x%lx - 0x%lx\n", start_addr, end_addr);
}

void remove_anti_memdump(void) {
    if (!start_addr || !end_addr)
        return;

    ((int (*)(unsigned long, int))rk_resolve_symbol("set_memory_rw"))(start_addr, (end_addr - start_addr) / PAGE_SIZE);
    ((int (*)(unsigned long, int))rk_resolve_symbol("set_memory_x"))(start_addr, (end_addr - start_addr) / PAGE_SIZE);

    printk(KERN_INFO "KPRZ: Anti-MemDump disabled.\n");
}
