#include "../include/vga.h"
#include "../include/timer.h"
#include "../include/memory.h"
#include "../include/pmm.h"
#include "../include/paging.h"

extern void idt_setup(); // sanity

void kmain() {
    vga_clear();
    vga_puts("Kernel: initializing...\n");

    // init PMM
    pmm_init();

    // enable paging
    paging_init();

    // print PMM stats
    char b[64];
    vga_puts("PMM pages total: ");
    // crude print of numbers (no printf)
    // We'll print free pages by polling pmm_free_pages
    uint32_t free = pmm_free_pages();
    // print free as hex:
    vga_puts("free pages: 0x");
    // print 8 hex digits
    const char *hex = "0123456789ABCDEF";
    for (int i = 7; i >= 0; --i) {
        uint8_t nib = (free >> (i*4)) & 0xF;
        vga_putc(hex[nib]);
    }
    vga_putc('\n');

    // initialize timer at 100Hz
    init_timer(100);
    vga_puts("Timer initialized.\n");

    vga_puts("Type on keyboard (basic mapping).\n");

    for(;;) {
        __asm__ volatile ("hlt");
    }
}
