#include "terminal.h"
#include "font.h"

static struct limine_framebuffer *term_fb = NULL;
static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;
static uint32_t term_color = 0xFFFFFF; // White text

void terminal_init(struct limine_framebuffer *fb) {
    term_fb = fb;
    cursor_x = 0;
    cursor_y = 0;
}

void terminal_putchar(char c) {
    if (!term_fb) return;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y += 8; // Our font is 8 pixels high
        return;
    }

    if (c == '\r') {
        cursor_x = 0;
        return;
    }

    // Handle wrapping
    if (cursor_x + 8 > term_fb->width) {
        cursor_x = 0;
        cursor_y += 8;
    }

    // Handle scrolling (very basic: just reset to top for now)
    if (cursor_y + 8 > term_fb->height) {
        cursor_x = 0;
        cursor_y = 0;
        // Basic screen clear could be done here, but skipped for simplicity
    }

    // Only print printable ASCII characters
    if ((unsigned char)c < 128) {
        uint32_t *fb_ptr = term_fb->address;
        char *glyph = font8x8_basic[(unsigned char)c];

        for (size_t y = 0; y < 8; y++) {
            for (size_t x = 0; x < 8; x++) {
                // The font bits are stored LSB to MSB or MSB to LSB depending on font.
                // font8x8 usually has LSB on the left. Let's check: 1 << x
                if ((glyph[y] >> x) & 1) {
                    uint32_t *pixel = fb_ptr + term_fb->pitch / 4 * (cursor_y + y) + (cursor_x + x);
                    *pixel = term_color;
                }
            }
        }
    }

    cursor_x += 8; // Move cursor forward
}

void terminal_print(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        terminal_putchar(str[i]);
    }
}
