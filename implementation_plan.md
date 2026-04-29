# VibeOS (x86_64) Implementation Plan

This plan details the foundation of **VibeOS**, a basic x86_64 operating system. 
We will use the **Limine** bootloader protocol to handle the transition into 64-bit long mode, paging setup, and framebuffer initialization, allowing us to focus purely on the kernel logic.

> [!WARNING]
> **Missing Toolchain Dependencies Detected**
> I've detected that your Windows environment does not have a C/C++ cross-compiler (`gcc`), `make`, `qemu-system-x86_64`, or Windows Subsystem for Linux (WSL) installed. Building a custom OS requires a Unix-like build environment.
> 
> **Recommendation:** The easiest way to build and test VibeOS on Windows is to install WSL (Ubuntu). You can do this by opening an Administrator PowerShell and running `wsl --install`, then restarting your PC.

## User Review Required

1. **Toolchain Installation:** Are you willing to install WSL to compile and run VibeOS? If so, I will provide the commands to install the necessary packages inside WSL once it's set up. Alternatively, if you prefer another method (like MSYS2), please let me know.
2. **Kernel Language:** I plan to write the kernel primarily in **C** with some inline Assembly where necessary. Is this acceptable?
3. **Features:** For this initial version, the kernel will halt gracefully and print a welcome message to the Limine framebuffer. Are there any other immediate features you want?

## Open Questions

- Do you have a preferred text editor or IDE setup within WSL, or will you continue using your Windows tools and just compile from the WSL terminal?
- Do you want to try writing a custom bootloader later on, or stick with Limine indefinitely?

## Proposed Changes

We will scaffold the following project structure:

```
VibeOS/
├── Makefile          # Build system configuration
├── limine.conf       # Limine bootloader configuration
├── linker.ld         # Linker script to organize the kernel executable
├── limine/           # Limine bootloader binaries and headers (to be downloaded)
└── src/
    ├── kernel.c      # Main kernel entry point
    └── limine.h      # Limine protocol header
```

### Core Implementation

#### [NEW] `src/kernel.c`
This file will contain the main entry point (`_start`) called by the Limine bootloader. It will:
- Request the framebuffer from Limine.
- Print a basic "Welcome to VibeOS!" message to the screen.
- Halt the CPU in an infinite loop safely.

#### [NEW] `linker.ld`
A linker script specifically designed for an x86_64 higher-half kernel. It will dictate the memory layout of our kernel executable (ELF64 format) so Limine knows how to load it correctly.

#### [NEW] `limine.conf`
The configuration file for Limine, specifying the timeout, boot entry name (VibeOS), and the path to our kernel executable.

#### [NEW] `Makefile`
Automates the compilation of `kernel.c`, linking it via `linker.ld`, and using `xorriso` to package the kernel and the Limine bootloader into a bootable `vibeos.iso` file.

## Verification Plan

### Automated Tests
Currently, there are no unit tests. Verification will be done by building the OS and running it in an emulator.

### Manual Verification
Once the toolchain (WSL/GCC/Make) is available:
1. Run `make` to compile the kernel and build `vibeos.iso`.
2. Run `make run` to boot `vibeos.iso` using QEMU.
3. Verify that the QEMU window appears and displays the "Welcome to VibeOS" message.
