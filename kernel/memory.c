#include "../include/memory.h"
#include "../include/pmm.h"
#include <stdint.h>

static uint32_t heap_start = 0x100000; // 1MB
static uint32_t heap_ptr = 0x100000;
static uint32_t heap_end = 0x200000;   // 2MB initial

void* kmalloc(uint32_t size) {
    // align size to 4
    if (size & 3) size = (size + 3) & ~3;
    uint32_t old = heap_ptr;
    heap_ptr += size;

    // if heap_ptr crosses into unbacked pages, allocate pages via PMM
    while (heap_ptr > heap_end) {
        void *p = pmm_alloc_page();
        if (!p) return 0; // OOM
        // Since we identity-mapped physical==virtual for first 16MB, no mapping needed
        heap_end += 4096;
    }
    return (void*)old;
}
