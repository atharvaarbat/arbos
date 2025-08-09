#include "../include/paging.h"
#include "../include/vga.h"
#include <stdint.h>

#define PDE_PRESENT 0x1
#define PDE_RW      0x2
#define PDE_USER    0x4

#define PTE_PRESENT 0x1
#define PTE_RW      0x2
#define PTE_USER    0x4

// We'll identity-map the first 16 MiB: 16MiB / 4MiB = 4 page tables (each 4MB)
static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t page_table0[1024] __attribute__((aligned(4096)));
static uint32_t page_table1[1024] __attribute__((aligned(4096)));
static uint32_t page_table2[1024] __attribute__((aligned(4096)));
static uint32_t page_table3[1024] __attribute__((aligned(4096)));

static inline void write_cr3(uint32_t v) {
    __asm__ volatile ("mov %0, %%cr3" : : "r"(v) : "memory");
}
static inline uint32_t read_cr0() {
    uint32_t v; __asm__ volatile ("mov %%cr0, %0" : "=r"(v)); return v;
}
static inline void write_cr0(uint32_t v) {
    __asm__ volatile ("mov %0, %%cr0" : : "r"(v) : "memory");
}

void paging_init() {
    // zero everything
    for (int i=0;i<1024;i++) page_directory[i] = 0;
    for (int i=0;i<1024;i++) page_table0[i] = page_table1[i] = page_table2[i] = page_table3[i] = 0;

    // fill page tables: each entry maps 4KB => covers 4MB per table
    uint32_t base = 0;
    uint32_t *tables[4] = { page_table0, page_table1, page_table2, page_table3 };
    for (int t=0; t<4; ++t) {
        for (int i=0;i<1024;++i) {
            tables[t][i] = (base & 0xFFFFF000) | PTE_PRESENT | PTE_RW;
            base += 0x1000; // next 4KB
        }
    }

    // point PDEs 0..3 to these tables
    page_directory[0] = ((uint32_t)page_table0) | PDE_PRESENT | PDE_RW;
    page_directory[1] = ((uint32_t)page_table1) | PDE_PRESENT | PDE_RW;
    page_directory[2] = ((uint32_t)page_table2) | PDE_PRESENT | PDE_RW;
    page_directory[3] = ((uint32_t)page_table3) | PDE_PRESENT | PDE_RW;

    // load page directory
    write_cr3((uint32_t)page_directory);

    // enable paging: set PG bit in CR0
    uint32_t cr0 = read_cr0();
    cr0 |= 0x80000000u;
    write_cr0(cr0);

    vga_puts("Paging: enabled (identity-mapped first 16MB)\n");
}
