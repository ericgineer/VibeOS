#include "gdt.h"

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct gdt_entry gdt[5];
struct gdt_ptr gdtr;

static void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[index].base_low = (base & 0xFFFF);
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;

    gdt[index].limit_low = (limit & 0xFFFF);
    gdt[index].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[index].access = access;
}

void gdt_init(void) {
    gdtr.limit = (sizeof(struct gdt_entry) * 5) - 1;
    gdtr.base = (uint64_t)&gdt;

    gdt_set_entry(0, 0, 0, 0, 0); // Null descriptor
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xA0); // Kernel Code (64-bit) (L bit set, D/B cleared -> 0xA0)
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xA0); // Kernel Data (64-bit)
    gdt_set_entry(3, 0, 0xFFFFF, 0xF2, 0xA0); // User Data (64-bit)
    gdt_set_entry(4, 0, 0xFFFFF, 0xFA, 0xA0); // User Code (64-bit)

    // Load GDT and reload segment registers
    asm volatile (
        "lgdt %0\n\t"
        "pushq $0x08\n\t"
        "lea 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n"
        "1:\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        :
        : "m"(gdtr)
        : "rax", "memory"
    );
}
