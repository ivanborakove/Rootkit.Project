
# K-PRZ Rootkit (Kernel-Mode Research Rootkit)

## Project Summary

**K-PRZ Rootkit** is a highly modular and research-focused **Linux Kernel-Mode Rootkit** developed for **cybersecurity lab environments**. Its design goal is to provide a framework for **defensive EDR testing**, **malware analysis**, and **antirootkit research**, focusing on **modern Linux kernel internals (5.x / 6.x series)**.

> **Legal Disclaimer:**  
This project is intended **strictly for controlled lab research and educational purposes**. Do not deploy on production systems. The authors and contributors hold **no responsibility for misuse**.

## Current Module Features

| Module                         | Purpose                          |
|--------------------------------|----------------------------------|
| **FTRACE HOOKING**             | Kernel function redirection via Ftrace |
| **INLINE HOOKING**             | Direct binary patching of kernel code |
| **SOFTLOCKUP WATCHDOG BYPASS** | Disables kernel softlockup detection |
| **REVERSE SHELL**              | Kernel-level reverse TCP shell |
| **KEYLOGGER**                  | Captures and stores keystrokes |
| **NETLINK EXFIL**              | Exfiltration via netlink sockets |
| **COMMAND EXECUTOR**           | Execute arbitrary kernel commands |
| **SELF-WATCHDOG**              | Prevents module unload by periodic refcount manipulation |

## Kernel Version Support

| Tested on Kernel Versions       | Notes                          |
|---------------------------------|-------------------------------|
| **5.7.x → 5.15.x**             | Full support |
| **6.0.x → 6.11.x (latest)**    | Includes special handling for ftrace_regs and kallsyms_lockdown |

Dynamic runtime detection for kernel API changes (e.g., `ftrace_get_regs_ip`, `kallsyms_lookup_name`) is built-in.

## Architecture Overview

### Symbol Resolution Abstraction (`memory_utils.c`)
- Uses **self-resolving kallsyms lookup**
- Supports kernels where direct use of `kallsyms_lookup_name` is blocked
- Provides:
```c
unsigned long rk_resolve_symbol(const char *name);
```

### Write Protection Handling (`write_protection.c`)
- Allows safe **temporary disabling of write-protected pages** during inline patching
- Uses **CR0 register manipulation**

### Ftrace Hook Engine (`ftrace_hook.c`, `ftrace_regs.c`)
- Maintains a **dynamic array of hooks**
- Exports core functions:
```c
fh_install_hooks()
fh_remove_hooks()
fh_set_hooks()
```
- Compatible with:

| Kernel Version       | Behavior                      |
|----------------------|-------------------------------|
| **< 6.7.x**          | Direct manipulation of `pt_regs->ip` |
| **≥ 6.7.x**          | Uses `ftrace_get_regs_ip` / `ftrace_set_regs_ip` |

### Inline Hook Engine (`inline_hook.c`)
- Direct **memory patching (x86 long jump patch)**
- Bypasses control flow by overwriting first few bytes of kernel functions

### Softlockup Watchdog Bypass (`watchdog.c`)
- Hooks the function `touch_softlockup_watchdog`
- Prevents kernel from detecting CPU hangs

### Module Manager (`rootkit_core.c`)
- Manages state of each feature/module  
- Provides runtime toggling:
```c
enable_module(enum rk_module_id id);
disable_module(enum rk_module_id id);
```
- Supports **Payload Triggering API**:
```c
trigger_payload(enum rk_payload_type type);
```

## Current Exported Symbols

| Function                          | Purpose                        |
|-----------------------------------|--------------------------------|
| `fh_install_hooks`                | Install multiple ftrace hooks |
| `fh_remove_hooks`                 | Remove multiple ftrace hooks |
| `rk_resolve_symbol`               | Dynamic kernel symbol resolution |
| `disable_write_protection`        | Turn off WP |
| `enable_write_protection`         | Turn on WP |
| `init_ftrace_regs` / `cleanup_ftrace_regs` | Setup ftrace regs access |
| `rk_ftrace_get_ip` / `rk_ftrace_set_ip` | IP access abstraction |

## Triggered Payloads (On-Demand)

| Payload                    | Behavior                |
|----------------------------|-------------------------|
| REVERSE_SHELL              | Kernel-level TCP shell |
| KEYLOGGER_DUMP             | Dump captured keystrokes |
| TRIGGER_RECOVERY           | Execute rootkit recovery logic |
| CUSTOM                     | Executes custom kernel command (ex: create temp file) |

## Building the Rootkit

```bash
make
```
The build uses:
- **Kernel headers from** `/lib/modules/$(uname -r)/build`
- Automatically adjusts flags for kernel version (via `LINUX_VERSION_CODE` checks)

## Clean Build

```bash
make clean
```

## Current .gitignore highlights:

- Ignores all:  
`.o`, `.ko`, `.mod.*`, `.symvers`, `.order`, `.markers`, `.cmd`, `.tmp_versions/`, `/build/`, `/out/`, editor files and packaging artifacts.
- Also ignores intermediate **dependency files like `.o.d`**.

## Next Steps (Suggested)

- Finish the **Netfilter trigger**
- Implement **stealth techniques (LKM hiding, syscall table patching, etc)**
- Add **LKM initialization loader with controlled userland trigger**

## Final Note:
name: Kernel Module Build CI

on:
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Install build dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y build-essential linux-headers-$(uname -r)

      - name: Configure Git user.name and user.email (noreply)
        run: |
          git config --global user.name ""
          git config --global user.email ""

      - name: Build Kernel Module
        run: |
          make

      - name: Verify build output
        run: |
          file *.ko

      - name: Commit changes with standard message
        run: |

Again, **this code is for LAB TESTING ONLY**.  
Do **not** deploy this on real systems.  
Use for **learning**, **detection engineering**, and **defensive product research**.
