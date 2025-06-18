#include "write_protection.h"
#include <linux/preempt.h>
#include <asm/paravirt.h>
#include <asm/processor.h>

#ifndef X86_CR0_WP
#define X86_CR0_WP (1UL << 16)
#endif

void disable_write_protection(void) {
    unsigned long cr0;

    preempt_disable();
    barrier();
    cr0 = read_cr0();
    write_cr0(cr0 & ~X86_CR0_WP);
    barrier();
}

void enable_write_protection(void) {
    unsigned long cr0;

    barrier();
    cr0 = read_cr0();
    write_cr0(cr0 | X86_CR0_WP);
    barrier();
    preempt_enable();
}

EXPORT_SYMBOL_GPL(disable_write_protection);
EXPORT_SYMBOL_GPL(enable_write_protection);
