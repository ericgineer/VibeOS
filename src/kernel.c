#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"
#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "pmm.h"

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

    // Test PMM
    void *page1 = pmm_alloc_page();
    void *page2 = pmm_alloc_page();

    terminal_print("Allocated Page 1 at: ");
    terminal_print_hex((uint64_t)page1);
    terminal_print("\n");

    terminal_print("Allocated Page 2 at: ");
    terminal_print_hex((uint64_t)page2);
    terminal_print("\n");

    pmm_free_page(page1);
    pmm_free_page(page2);
    terminal_print("Pages freed successfully.\n");

    // We're done, just halt...
    hcf();
}
