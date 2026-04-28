#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"

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

    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    uint32_t *fb_ptr = framebuffer->address;

    // Draw a green square in the top-left corner
    for (size_t i = 0; i < 200; i++) {
        for (size_t j = 0; j < 200; j++) {
            uint32_t *pixel = fb_ptr + framebuffer->pitch / 4 * i + j;
            *pixel = 0x00FF00; // Green
        }
    }

    // Draw a blue square next to it
    for (size_t i = 0; i < 200; i++) {
        for (size_t j = 200; j < 400; j++) {
            uint32_t *pixel = fb_ptr + framebuffer->pitch / 4 * i + j;
            *pixel = 0x0000FF; // Blue
        }
    }

    // We're done, just halt...
    hcf();
}
