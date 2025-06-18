#include "anti_watchdog.h"
#include "ftrace_hook.h"

static void (*real_touch_softlockup_watchdog)(void);

static void hooked_touch_softlockup_watchdog(void) {
    return;
}

static struct ftrace_hook watchdog_hook = {
    .name = "touch_softlockup_watchdog",
    .function = hooked_touch_softlockup_watchdog,
    .original = &real_touch_softlockup_watchdog,
};

void setup_anti_watchdog(void) {
    fh_install_hook(&watchdog_hook);
}

void remove_anti_watchdog(void) {
    fh_remove_hook(&watchdog_hook);
}
