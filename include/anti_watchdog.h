#ifndef ANTI_WATCHDOG_H
#define ANTI_WATCHDOG_H

#include <linux/linkage.h>

extern asmlinkage void hooked_touch_softlockup_watchdog(void);
extern asmlinkage void (*real_touch_softlockup_watchdog)(void);

void setup_anti_watchdog(void);
void remove_anti_watchdog(void);

#endif
