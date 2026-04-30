#include "keyboard.h"
#include "io.h"
#include "terminal.h"

// Basic PS/2 Set 1 US QWERTY lowercase map
const char kbd_US[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);
    // If top bit is set, it's a key release, ignore for now
    if (!(scancode & 0x80)) {
        char c = kbd_US[scancode];
        if (c) {
            terminal_putchar(c);
        }
    }
}

void keyboard_init(void) {
    terminal_print("Keyboard driver initialized.\n");
}
