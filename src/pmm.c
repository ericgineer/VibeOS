#include "pmm.h"
#include "limine.h"
#include "terminal.h"

static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

uint64_t pmm_hhdm_offset = 0;

static uint8_t *bitmap = NULL;
static size_t bitmap_size = 0;
static size_t highest_page = 0;

static inline void bitmap_set(size_t bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

static inline void bitmap_clear(size_t bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static inline int bitmap_test(size_t bit) {
    return (bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

void pmm_init(void) {
    struct limine_memmap_response *memmap = memmap_request.response;
    struct limine_hhdm_response *hhdm = hhdm_request.response;

    if (!memmap || !hhdm) {
        terminal_print("PMM: Failed to get Limine requests!\n");
        return;
    }

    pmm_hhdm_offset = hhdm->offset;
    uint64_t highest_addr = 0;

    // Find highest memory address
    for (size_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            uint64_t top = entry->base + entry->length;
            if (top > highest_addr) highest_addr = top;
        }
    }

    highest_page = highest_addr / PAGE_SIZE;
    bitmap_size = highest_page / 8;
    if (bitmap_size * 8 < highest_page) bitmap_size++; // Align up

    // Find a usable region large enough for the bitmap
    for (size_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= bitmap_size) {
            bitmap = (uint8_t *)(entry->base + pmm_hhdm_offset);
            break;
        }
    }

    if (!bitmap) {
        terminal_print("PMM: Failed to allocate bitmap!\n");
        return;
    }

    // Initialize bitmap (set all to 1 = used initially)
    for (size_t i = 0; i < bitmap_size; i++) {
        bitmap[i] = 0xFF;
    }

    // Iterate through memmap and free usable regions
    for (size_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            for (uint64_t addr = entry->base; addr < entry->base + entry->length; addr += PAGE_SIZE) {
                bitmap_clear(addr / PAGE_SIZE);
            }
        }
    }

    // Now re-reserve the memory used by the bitmap itself!
    uint64_t bitmap_phys = (uint64_t)bitmap - pmm_hhdm_offset;
    for (uint64_t addr = bitmap_phys; addr < bitmap_phys + bitmap_size; addr += PAGE_SIZE) {
        bitmap_set(addr / PAGE_SIZE);
    }
    
    // Also reserve page 0 (null pointer)
    bitmap_set(0);

    terminal_print("PMM: Initialized. Highest Address: ");
    terminal_print_hex(highest_addr);
    terminal_print("\n");
}

void *pmm_alloc_pages(size_t count) {
    if (count == 0) return NULL;
    
    size_t run = 0;
    size_t start_bit = 0;

    for (size_t i = 0; i < highest_page; i++) {
        if (!bitmap_test(i)) {
            if (run == 0) start_bit = i;
            run++;
            if (run == count) {
                for (size_t j = 0; j < count; j++) {
                    bitmap_set(start_bit + j);
                }
                return (void *)(start_bit * PAGE_SIZE);
            }
        } else {
            run = 0;
        }
    }

    return NULL; // Out of memory
}

void *pmm_alloc_page(void) {
    return pmm_alloc_pages(1);
}

void pmm_free_pages(void *page, size_t count) {
    size_t start_bit = (uint64_t)page / PAGE_SIZE;
    for (size_t i = 0; i < count; i++) {
        bitmap_clear(start_bit + i);
    }
}

void pmm_free_page(void *page) {
    pmm_free_pages(page, 1);
}
