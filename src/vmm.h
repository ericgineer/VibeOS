#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#define PAGE_PRESENT  0x01
#define PAGE_WRITE    0x02
#define PAGE_USER     0x04

void vmm_init(void);
void vmm_map_page(uint64_t *pml4, uint64_t vaddr, uint64_t paddr, uint64_t flags);
uint64_t *vmm_get_current_pml4(void);

#endif
