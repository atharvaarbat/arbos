#include "../include/exceptions.h"
#include "../include/vga.h"
#include <stdint.h>

static const char *exception_messages[32] = {
    "Divide Error",                 /* 0 */
    "Debug",                        /* 1 */
    "Non-Maskable Interrupt",       /* 2 */
    "Breakpoint",                   /* 3 */
    "Into Detected Overflow",       /* 4 */
    "Out of Bounds",                /* 5 */
    "Invalid Opcode",               /* 6 */
    "No Coprocessor",               /* 7 */
    "Double Fault",                 /* 8 */
    "Coprocessor Segment Overrun",  /* 9 */
    "Bad TSS",                      /* 10 */
    "Segment Not Present",          /* 11 */
    "Stack Fault",                  /* 12 */
    "General Protection",           /* 13 */
    "Page Fault",                   /* 14 */
    "Reserved",                     /* 15 */
    "Floating Point Error",         /* 16 */
    "Alignment Check",              /* 17 */
    "Machine Check",                /* 18 */
    "SIMD Floating-Point",          /* 19 */
    "Virtualization Exception",     /* 20 */
    "Control Protection",           /* 21 */
    "Reserved",                     /* 22 */
    "Reserved",                     /* 23 */
    "Reserved",                     /* 24 */
    "Reserved",                     /* 25 */
    "Reserved",                     /* 26 */
    "Reserved",                     /* 27 */
    "Reserved",                     /* 28 */
    "Reserved",                     /* 29 */
    "Reserved",                     /* 30 */
    "Reserved"                      /* 31 */
};

/* regparm(2): first arg in EAX, second arg in EDX on i386 */
void exception_handler_regparm(int vec, uint32_t err) __attribute__((regparm(2)));

void exception_handler_regparm(int vec, uint32_t err) {
    vga_puts("\n[Exception] Vector: 0x");
    const char *hex = "0123456789ABCDEF";
    vga_putc(hex[(vec >> 4) & 0xF]);
    vga_putc(hex[vec & 0xF]);
    vga_puts(" - ");

    if (vec < 32 && exception_messages[vec]) {
        vga_puts(exception_messages[vec]);
    } else {
        vga_puts("Unknown");
    }

    vga_puts("\n");

    if (vec == 14) { /* Page Fault — read CR2 */
        uint32_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        vga_puts("CR2 = 0x");
        for (int i = 7; i >= 0; --i) {
            uint8_t nib = (cr2 >> (i*4)) & 0xF;
            vga_putc(hex[nib]);
        }
        vga_puts("\n");
    }

    /* If there is an error code, print it */
    vga_puts("Error code: 0x");
    for (int i = 7; i >= 0; --i) {
        uint8_t nib = (err >> (i*4)) & 0xF;
        vga_putc(hex[nib]);
    }
    vga_puts("\nSystem Halted.\n");

    for (;;) asm volatile ("cli; hlt");
}
