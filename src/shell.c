#include "shell.h"
#include "terminal.h"
#include <stddef.h>

#define BUFFER_SIZE 256
static char buffer[BUFFER_SIZE];
static int buffer_len = 0;

static int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static void shell_evaluate(void) {
    if (buffer_len == 0) return;
    
    // Null terminate the buffer for processing
    buffer[buffer_len] = '\0';
    
    if (strcmp(buffer, "help") == 0) {
        terminal_print("Available commands:\n");
        terminal_print("  help  - Show this help message\n");
        terminal_print("  clear - Clear the terminal screen\n");
        terminal_print("  echo  - Print text back to the terminal\n");
    } else if (strcmp(buffer, "clear") == 0) {
        terminal_clear();
    } else if (strncmp(buffer, "echo ", 5) == 0) {
        terminal_print(buffer + 5);
        terminal_print("\n");
    } else {
        terminal_print("Unknown command: ");
        terminal_print(buffer);
        terminal_print("\n");
    }
}

void shell_init(void) {
    terminal_clear();
    terminal_print("Welcome to VibeOS!\n");
    terminal_print("Type 'help' for a list of commands.\n\n");
    terminal_print("> ");
}

void shell_handle_input(char c) {
    if (c == '\n') {
        terminal_putchar('\n');
        shell_evaluate();
        buffer_len = 0;
        terminal_print("> ");
    } else if (c == '\b') {
        if (buffer_len > 0) {
            buffer_len--;
            terminal_putchar('\b');
        }
    } else if (c >= 32 && c <= 126) {
        if (buffer_len < BUFFER_SIZE - 1) {
            buffer[buffer_len++] = c;
            terminal_putchar(c);
        }
    }
}
