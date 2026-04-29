#include "isr.h"
#include "terminal.h"

const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

void isr_handler(struct registers *regs) {
    terminal_print("\n*** KERNEL PANIC ***\n");
    if (regs->int_no < 32) {
        terminal_print(exception_messages[regs->int_no]);
    } else {
        terminal_print("Unknown Exception");
    }
    terminal_print("\n\nRegister Dump:\n");
    terminal_print("RAX: "); terminal_print_hex(regs->rax); terminal_print("  RBX: "); terminal_print_hex(regs->rbx); terminal_print("\n");
    terminal_print("RCX: "); terminal_print_hex(regs->rcx); terminal_print("  RDX: "); terminal_print_hex(regs->rdx); terminal_print("\n");
    terminal_print("RSI: "); terminal_print_hex(regs->rsi); terminal_print("  RDI: "); terminal_print_hex(regs->rdi); terminal_print("\n");
    terminal_print("RBP: "); terminal_print_hex(regs->rbp); terminal_print("  RSP: "); terminal_print_hex(regs->rsp); terminal_print("\n");
    terminal_print("R8:  "); terminal_print_hex(regs->r8);  terminal_print("  R9:  "); terminal_print_hex(regs->r9); terminal_print("\n");
    terminal_print("R10: "); terminal_print_hex(regs->r10); terminal_print("  R11: "); terminal_print_hex(regs->r11); terminal_print("\n");
    terminal_print("R12: "); terminal_print_hex(regs->r12); terminal_print("  R13: "); terminal_print_hex(regs->r13); terminal_print("\n");
    terminal_print("R14: "); terminal_print_hex(regs->r14); terminal_print("  R15: "); terminal_print_hex(regs->r15); terminal_print("\n");
    terminal_print("RIP: "); terminal_print_hex(regs->rip); terminal_print("  RFLAGS: "); terminal_print_hex(regs->rflags); terminal_print("\n");
    terminal_print("CS:  "); terminal_print_hex(regs->cs);  terminal_print("  Error Code: "); terminal_print_hex(regs->err_code); terminal_print("\n");

    asm volatile("cli");
    for (;;) {
        asm volatile("hlt");
    }
}
