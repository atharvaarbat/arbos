#ifndef PMM_H
#define PMM_H

#include <stdint.h>

void pmm_init();
void *pmm_alloc_page();         // returns physical address as pointer (4KB)
void pmm_free_page(void *p);
uint32_t pmm_total_pages();
uint32_t pmm_free_pages();

#endif // PMM_H
