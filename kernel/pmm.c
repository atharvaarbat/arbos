#include "../include/pmm.h"
#include "../include/vga.h"
#include <stdint.h>

extern uint32_t end; // linker must provide this symbol at kernel end

// Configuration
#define PMM_MAX_PHYS_MB 64u
#define PAGE_SIZE 4096u
#define PMM_MAX_PAGES ((PMM_MAX_PHYS_MB * 1024u * 1024u) / PAGE_SIZE)

static uint8_t pmm_bitmap[(PMM_MAX_PAGES + 7) / 8] __attribute__((aligned(4096)));
static uint32_t total_pages = PMM_MAX_PAGES;
static uint32_t free_pages = 0;

static inline void bit_set(uint32_t n)   { pmm_bitmap[n >> 3] |= (1 << (n & 7)); }
static inline void bit_clear(uint32_t n) { pmm_bitmap[n >> 3] &= ~(1 << (n & 7)); }
static inline int  bit_test(uint32_t n)  { return (pmm_bitmap[n >> 3] >> (n & 7)) & 1; }

void pmm_init() {
    // Clear bitmap
    for (uint32_t i = 0; i < (sizeof(pmm_bitmap)); ++i) pmm_bitmap[i] = 0;

    // Initially mark all pages free
    free_pages = total_pages;

    // Mark low memory (0..1MB) as used (reserved)
    uint32_t reserved_pages = (0x100000u / PAGE_SIZE);
    for (uint32_t i = 0; i < reserved_pages; ++i) {
        bit_set(i);
        --free_pages;
    }

    // Mark kernel area as used: from 1MB up to end symbol (rounded up)
    uint32_t kernel_end_addr = ((uint32_t)&end);
    // If end < 1MB, clamp to 1MB
    if (kernel_end_addr < 0x100000u) kernel_end_addr = 0x100000u;
    uint32_t kernel_pages = (kernel_end_addr + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint32_t i = reserved_pages; i < kernel_pages && i < total_pages; ++i) {
        if (!bit_test(i)) { bit_set(i); --free_pages; }
    }

    // Mark bitmap storage itself as used (to avoid allocating pages holding the bitmap)
    uint32_t bmp_start = ((uint32_t)pmm_bitmap);
    uint32_t bmp_pages = ( (bmp_start + sizeof(pmm_bitmap) + PAGE_SIZE -1) / PAGE_SIZE );
    for (uint32_t i = 0; i < bmp_pages && i < total_pages; ++i) {
        if (!bit_test(i)) { bit_set(i); --free_pages; }
    }

    // print some info
    char buf[64];
    vga_puts("PMM: initialized\n");
    // don't use sprintf; print totals
}

void *pmm_alloc_page() {
    for (uint32_t i = 0; i < total_pages; ++i) {
        if (!bit_test(i)) {
            bit_set(i);
            --free_pages;
            return (void *)(i * PAGE_SIZE);
        }
    }
    return 0; // out of memory
}

void pmm_free_page(void *p) {
    if (!p) return;
    uint32_t addr = (uint32_t)p;
    uint32_t idx = addr / PAGE_SIZE;
    if (idx < total_pages && bit_test(idx)) {
        bit_clear(idx);
        ++free_pages;
    }
}

uint32_t pmm_total_pages() { return total_pages; }
uint32_t pmm_free_pages()  { return free_pages; }
