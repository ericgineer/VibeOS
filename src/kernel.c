#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"
#include "terminal.h"
#include "gdt.h"
#include "idt.h"

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

    terminal_print("Triggering a Divide By Zero Exception to test IDT...\n");
    
    // Trigger divide by zero
    volatile int a = 1;
    volatile int b = 0;
    volatile int c = a / b;
    (void)c;

    // We're done, just halt...
    hcf();
}
