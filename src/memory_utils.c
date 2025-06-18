#include "memory_utils.h"
#include <linux/version.h>
#include <linux/kallsyms.h>
#include <linux/export.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)

static unsigned long (*kallsyms_lookup_name_ptr)(const char *name) = NULL;

static int lookup_kallsyms_address(void) {
    unsigned long (*temp_lookup)(const char *name);


#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,7,0)
    temp_lookup = (void *)__symbol_get("kallsyms_lookup_name");
#else
    temp_lookup = (void *)kallsyms_lookup_name;
#endif

    if (!temp_lookup)
        return -1;

    kallsyms_lookup_name_ptr = temp_lookup;
    return 0;
}

unsigned long rk_resolve_symbol(const char *name) {
    if (!kallsyms_lookup_name_ptr) {
        if (lookup_kallsyms_address() != 0)
            return 0;
    }
    return kallsyms_lookup_name_ptr(name);
}

#else


unsigned long rk_resolve_symbol(const char *name) {
    return kallsyms_lookup_name(name);
}

#endif

EXPORT_SYMBOL_GPL(rk_resolve_symbol);
