#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"
#include "terminal.h"

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

    // Print a welcome message!
    terminal_print("Hello, VibeOS!\n");
    terminal_print("Text rendering is online and working pixel by pixel.\n");
    terminal_print("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
    terminal_print("abcdefghijklmnopqrstuvwxyz\n");
    terminal_print("0123456789 !@#$%^&*()\n");

    // We're done, just halt...
    hcf();
}
