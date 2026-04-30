#include "vmm.h"
#include "pmm.h"
#include "terminal.h"

#define PAGE_MASK 0xFFFFFFFFFFFFF000

uint64_t *vmm_get_current_pml4(void) {
    uint64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    return (uint64_t *)(cr3 + pmm_hhdm_offset);
}

void vmm_map_page(uint64_t *pml4, uint64_t vaddr, uint64_t paddr, uint64_t flags) {
    size_t pml4_index = (vaddr >> 39) & 0x1FF;
    size_t pdpt_index = (vaddr >> 30) & 0x1FF;
    size_t pd_index   = (vaddr >> 21) & 0x1FF;
    size_t pt_index   = (vaddr >> 12) & 0x1FF;

    if (!(pml4[pml4_index] & PAGE_PRESENT)) {
        uint64_t *new_pdpt = pmm_alloc_page();
        pml4[pml4_index] = (uint64_t)new_pdpt | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        uint64_t *v_pdpt = (uint64_t *)((uint64_t)new_pdpt + pmm_hhdm_offset);
        for(int i=0; i<512; i++) v_pdpt[i] = 0;
    }
    
    uint64_t *pdpt = (uint64_t *)((pml4[pml4_index] & PAGE_MASK) + pmm_hhdm_offset);

    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
        uint64_t *new_pd = pmm_alloc_page();
        pdpt[pdpt_index] = (uint64_t)new_pd | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        uint64_t *v_pd = (uint64_t *)((uint64_t)new_pd + pmm_hhdm_offset);
        for(int i=0; i<512; i++) v_pd[i] = 0;
    }

    uint64_t *pd = (uint64_t *)((pdpt[pdpt_index] & PAGE_MASK) + pmm_hhdm_offset);

    if (!(pd[pd_index] & PAGE_PRESENT)) {
        uint64_t *new_pt = pmm_alloc_page();
        pd[pd_index] = (uint64_t)new_pt | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        uint64_t *v_pt = (uint64_t *)((uint64_t)new_pt + pmm_hhdm_offset);
        for(int i=0; i<512; i++) v_pt[i] = 0;
    }

    uint64_t *pt = (uint64_t *)((pd[pd_index] & PAGE_MASK) + pmm_hhdm_offset);

    pt[pt_index] = paddr | flags;
}

void vmm_init(void) {
    terminal_print("VMM: Initialized. Modifying Limine's active page tables.\n");
}
