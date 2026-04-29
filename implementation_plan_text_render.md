# VibeOS Text Renderer Implementation Plan

This plan outlines the addition of a basic framebuffer text renderer to VibeOS. Since Limine gives us a pixel-based framebuffer (not VGA text mode), we must manually draw pixels to form characters. We will implement an 8x16 bitmap font and a basic terminal driver.

## User Review Required

> [!NOTE]
> We will be adding a hardcoded 8x16 PC terminal font into the kernel to keep things simple for now. Later, we can load fonts dynamically from the filesystem, but embedding it directly into the kernel is standard practice for early OS development.

## Proposed Changes

### Terminal Core

#### [NEW] `src/font.h`
Will contain a raw `unsigned char` array representing the standard 8x16 VGA text font. Each character is 16 bytes, where each bit in a byte represents a pixel (on/off) in a row.

#### [NEW] `src/terminal.h`
Header file for our terminal driver. It will expose the following API:
- `void terminal_init(struct limine_framebuffer *fb);`
- `void terminal_putchar(char c);`
- `void terminal_print(const char *str);`

#### [NEW] `src/terminal.c`
The implementation of the text renderer. It will:
- Store the Limine framebuffer pointer globally.
- Keep track of the current `cursor_x` and `cursor_y` positions.
- Handle basic logic for newlines (`\n`) and wrapping text when it reaches the edge of the screen.

### Kernel Updates

#### [MODIFY] `src/kernel.c`
We will replace the green and blue square drawing code with a call to initialize the terminal and print a welcome message (e.g., `terminal_print("Hello, VibeOS!\nText rendering is online.");`).

#### [MODIFY] `Makefile`
Update the build script to compile `src/terminal.c` alongside `kernel.c`.

## Verification Plan

### Automated Tests
- Run `wsl make` to ensure the project compiles without warnings or linking errors.

### Manual Verification
- Run `make run` in QEMU. We expect to see crisp, white text on the screen instead of the colored squares.
