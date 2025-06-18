#include "packet_filter.h"
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>

static struct nf_hook_ops packet_filter_ops;

static unsigned int packet_filter_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *ip_header;
    struct tcphdr *tcp_header;

    if (!skb)
        return NF_ACCEPT;

    ip_header = ip_hdr(skb);
    if (ip_header->protocol == IPPROTO_TCP) {
        tcp_header = tcp_hdr(skb);
        if (ntohs(tcp_header->dest) == 9999)
            return NF_DROP;
    }

    return NF_ACCEPT;
}

void setup_packet_filter(void) {
    packet_filter_ops.hook = packet_filter_hook;
    packet_filter_ops.pf = PF_INET;
    packet_filter_ops.hooknum = NF_INET_PRE_ROUTING;
    packet_filter_ops.priority = NF_IP_PRI_FIRST;
    nf_register_net_hook(&init_net, &packet_filter_ops);
}

void remove_packet_filter(void) {
    nf_unregister_net_hook(&init_net, &packet_filter_ops);
}
