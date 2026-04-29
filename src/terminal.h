#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <stddef.h>
#include "limine.h"

void terminal_init(struct limine_framebuffer *fb);
void terminal_putchar(char c);
void terminal_print(const char *str);
void terminal_print_hex(uint64_t val);

#endif // TERMINAL_H
