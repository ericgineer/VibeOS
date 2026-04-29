# VibeOS CPU Initialization Plan (GDT & IDT)

This plan outlines the steps to initialize the core CPU data structures for VibeOS in 64-bit Long Mode: the Global Descriptor Table (GDT) and the Interrupt Descriptor Table (IDT).

While Limine sets up a temporary GDT for us, building our own is essential for future features (like user-space programs, which require a Task State Segment). The IDT is absolutely critical—without it, any CPU exception (like dividing by zero or a page fault) will cause the CPU to triple-fault and instantly reboot the machine.

## User Review Required

> [!IMPORTANT]
> Interrupt handling in x86_64 requires saving the CPU registers before calling C code. We will introduce our first pure assembly file (`src/isr_stubs.S`) to handle this context saving. We will also update the `Makefile` to support compiling `.S` (Assembly) files.

## Open Questions
- We will set up the first 32 CPU exceptions (Page Fault, General Protection Fault, etc.) to print a "Blue Screen of Death" style panic message to our new terminal. Do you want to include register dumps (showing the exact values of RAX, RBX, etc. when the crash happened) in the panic screen right now, or keep it simple?

## Proposed Changes

### 1. Global Descriptor Table (GDT)
#### [NEW] `src/gdt.h` & `src/gdt.c`
We will define a 64-bit GDT containing 5 entries:
1. Null Descriptor
2. Kernel Code Segment
3. Kernel Data Segment
4. User Data Segment
5. User Code Segment

We will use inline assembly to run the `lgdt` instruction and perform a far return to reload the `CS` (Code Segment) register.

### 2. Interrupt Descriptor Table (IDT)
#### [NEW] `src/idt.h` & `src/idt.c`
We will define an array of 256 IDT entries (each 16 bytes long, as required by x86_64). We will write a function `idt_set_gate()` to easily map an interrupt number to a handler function.

### 3. Interrupt Service Routines (ISRs)
#### [NEW] `src/isr_stubs.S`
Assembly stubs for the 32 CPU exceptions. Since x86_64 removed the `pusha` instruction, we will manually write macros to push all registers onto the stack, call our C handler, and pop them back off before calling `iretq` (Interrupt Return).

#### [NEW] `src/isr.h` & `src/isr.c`
The central C handler that receives the interrupt. If a CPU exception occurs, this function will use our `terminal_print` function to print the fault name (e.g., "Divide by Zero Error") and halt the system.

### 4. Integration
#### [MODIFY] `src/kernel.c`
Call `gdt_init()` and `idt_init()` right after setting up the terminal. We will intentionally trigger a divide-by-zero error afterward to test the IDT.

#### [MODIFY] `Makefile`
Add a build rule for `%.o: %.S` so that our assembly files are assembled and linked into the kernel.

## Verification Plan

### Automated Tests
- Run `wsl make` to ensure all new C and Assembly files compile and link successfully without PIE/relocation errors.

### Manual Verification
- We will add `int a = 1 / 0;` (or an explicit `asm("int $0x0")`) in `kernel.c`.
- Run `make run` in QEMU.
- Expectation: Instead of the emulator constantly rebooting, the VibeOS terminal should display a formatted exception message and halt gracefully.
