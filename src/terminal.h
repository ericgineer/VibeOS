#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <stddef.h>
#include "limine.h"

void terminal_init(struct limine_framebuffer *fb);
void terminal_putchar(char c);
void terminal_print(const char *str);

#endif // TERMINAL_H
