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

static void terminal_scroll(void) {
    uint32_t *fb_ptr = term_fb->address;
    size_t row_stride = term_fb->pitch / 4;
    size_t shift_rows = 8;
    
    // Move everything up by shift_rows
    for (size_t y = shift_rows; y < term_fb->height; y++) {
        for (size_t x = 0; x < term_fb->width; x++) {
            fb_ptr[row_stride * (y - shift_rows) + x] = fb_ptr[row_stride * y + x];
        }
    }
    
    // Clear bottom shift_rows
    for (size_t y = term_fb->height - shift_rows; y < term_fb->height; y++) {
        for (size_t x = 0; x < term_fb->width; x++) {
            fb_ptr[row_stride * y + x] = 0x000000;
        }
    }
    
    cursor_y -= shift_rows;
}

void terminal_clear(void) {
    if (!term_fb) return;
    uint32_t *fb_ptr = term_fb->address;
    size_t row_stride = term_fb->pitch / 4;
    for (size_t y = 0; y < term_fb->height; y++) {
        for (size_t x = 0; x < term_fb->width; x++) {
            fb_ptr[row_stride * y + x] = 0x000000;
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void terminal_putchar(char c) {
    if (!term_fb) return;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y += 8; // Our font is 8 pixels high
        if (cursor_y + 8 > term_fb->height) terminal_scroll();
        return;
    }

    if (c == '\r') {
        cursor_x = 0;
        return;
    }

    if (c == '\b') {
        if (cursor_x >= 8) {
            cursor_x -= 8;
        } else if (cursor_y >= 8) {
            cursor_y -= 8;
            cursor_x = term_fb->width - (term_fb->width % 8) - 8;
        }
        uint32_t *fb_ptr = term_fb->address;
        size_t row_stride = term_fb->pitch / 4;
        for (size_t y = 0; y < 8; y++) {
            for (size_t x = 0; x < 8; x++) {
                fb_ptr[row_stride * (cursor_y + y) + (cursor_x + x)] = 0x000000;
            }
        }
        return;
    }

    // Handle wrapping
    if (cursor_x + 8 > term_fb->width) {
        cursor_x = 0;
        cursor_y += 8;
    }

    // Handle scrolling
    if (cursor_y + 8 > term_fb->height) {
        terminal_scroll();
    }

    // Only print printable ASCII characters
    if ((unsigned char)c < 128) {
        uint32_t *fb_ptr = term_fb->address;
        char *glyph = font8x8_basic[(unsigned char)c];
        size_t row_stride = term_fb->pitch / 4;

        for (size_t y = 0; y < 8; y++) {
            for (size_t x = 0; x < 8; x++) {
                if ((glyph[y] >> x) & 1) {
                    uint32_t *pixel = fb_ptr + row_stride * (cursor_y + y) + (cursor_x + x);
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

void terminal_print_hex(uint64_t val) {
    char buf[17];
    buf[16] = '\0';
    for (int i = 15; i >= 0; i--) {
        int nibble = val & 0xF;
        if (nibble < 10) buf[i] = '0' + nibble;
        else buf[i] = 'A' + (nibble - 10);
        val >>= 4;
    }
    terminal_print("0x");
    terminal_print(buf);
}
