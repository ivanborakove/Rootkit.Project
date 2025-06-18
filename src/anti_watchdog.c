#include "ftrace_hook.h"
#include "anti_watchdog.h"

static struct fh_hook watchdog_hook = {
    .name = "touch_softlockup_watchdog",
    .func = hooked_touch_softlockup_watchdog,
    .orig = &real_touch_softlockup_watchdog,
};

void setup_anti_watchdog(void) {
    fh_install_all();
}

void remove_anti_watchdog(void) {
    fh_remove_all();
}
