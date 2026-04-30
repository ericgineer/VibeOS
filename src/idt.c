#include "idt.h"

struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  ist;
    uint8_t  flags;
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t always0;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtr;

extern uint64_t isr_stub_table[];

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_mid = (base >> 16) & 0xFFFF;
    idt[num].base_high = (base >> 32) & 0xFFFFFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
    idt[num].ist = 0;
}

void idt_init(void) {
    idtr.limit = sizeof(struct idt_entry) * 256 - 1;
    idtr.base = (uint64_t)&idt;

    for (int i = 0; i < 48; i++) {
        idt_set_gate(i, isr_stub_table[i], 0x08, 0x8E); // 0x8E = Interrupt Gate, ring 0
    }

    asm volatile("lidt %0" : : "m"(idtr));
    asm volatile("sti"); // Enable interrupts
}
