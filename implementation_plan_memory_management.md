# VibeOS Physical Memory Manager (PMM) Plan

This plan outlines the implementation of the Physical Memory Manager (PMM). The PMM is responsible for keeping track of all the physical RAM installed in the system and handing out 4KB chunks (called "pages") to the rest of the kernel upon request.

## User Review Required

> [!NOTE]
> We will use a **Bitmap Allocator** for the PMM. This is the industry standard for early OS development because it is relatively simple to implement and uses very little memory (1 bit represents a 4KB page, meaning tracking 4GB of RAM requires only 128KB for the bitmap).

## Proposed Changes

### 1. Limine Memory Map Integration
We will define a Limine Memory Map request in our PMM. This allows the kernel to ask Limine, "What memory is installed, and what areas are safe to use?" Limine will return an array of memory regions, telling us exactly where the RAM is and what regions are reserved by hardware (like ACPI or video memory).

### 2. Physical Memory Manager Core
#### [NEW] `src/pmm.h`
Will expose the PMM API:
- `void pmm_init(void);`
- `void* pmm_alloc_page(void);`
- `void* pmm_alloc_pages(size_t count);`
- `void pmm_free_page(void *page);`
- `void pmm_free_pages(void *page, size_t count);`

#### [NEW] `src/pmm.c`
The core implementation. `pmm_init()` will:
1. Iterate over the Limine memory map to find the highest memory address (to know how large the bitmap needs to be).
2. Find a large enough `USABLE` chunk of memory in the map to store the bitmap itself.
3. Initialize the bitmap: first mark *everything* as used, then iterate over the map again to mark only `USABLE` regions as "free" (0).
4. `pmm_alloc_page()` will scan the bitmap for the first `0` bit, flip it to `1`, and return the physical address of that 4KB page.

### 3. Integration & Testing
#### [MODIFY] `src/kernel.c`
We will remove the intentional Division By Zero crash. Instead, we will call `pmm_init()` and then run a simple test: allocate a few pages using `pmm_alloc_page()`, print their physical memory addresses to the terminal to verify they are distinct 4KB-aligned blocks, and then free them.

#### [MODIFY] `Makefile`
Add `pmm.c` to the build script so it links correctly.

## Verification Plan

### Automated Tests
- Run `wsl make` to ensure `pmm.c` compiles and links correctly.

### Manual Verification
- Run `make run` in QEMU.
- Expectation: The terminal should print the initialization of the PMM, display how much usable RAM was found, and print the physical addresses of successfully allocated test pages (e.g., `0x1000`, `0x2000`).
