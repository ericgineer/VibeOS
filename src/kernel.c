#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"
#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "pmm.h"
#include "vmm.h"
#include "pic.h"
#include "keyboard.h"

// Set the base revision to 2, this is recommended.
LIMINE_BASE_REVISION(2)

// Request a framebuffer.
struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// Halt and catch fire function.
static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

// The entry point for our kernel.
void _start(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // Initialize the terminal renderer
    terminal_init(framebuffer);

    terminal_print("Initializing GDT...\n");
    gdt_init();

    terminal_print("Initializing IDT...\n");
    idt_init();

    terminal_print("Initializing PMM...\n");
    pmm_init();

    terminal_print("Initializing VMM...\n");
    vmm_init();

    terminal_print("Remapping PIC...\n");
    pic_remap(32, 40);
    pic_clear_masks(); // Unmask all IRQs

    terminal_print("Initializing Keyboard...\n");
    keyboard_init();

    terminal_print("\nVibeOS is now running. Try typing something!\n> ");

    // We're done initializing, enter infinite loop.
    // Make sure interrupts stay enabled so keyboard works.
    for (;;) {
        asm ("hlt");
    }
}
