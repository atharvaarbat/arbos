#include "../include/keyboard.h"
#include "../include/vga.h"

static inline unsigned char inb(unsigned short port) {
    unsigned char data;
    __asm__ volatile ("inb %1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

void keyboard_handler_c() {
    unsigned char scancode = inb(0x60);
    // very small scancode -> ASCII map for 1..26 -> a..z (simplified)
    if (scancode >= 0x02 && scancode <= 0x0B) {
        // number row 1..0 map
        const char *nums = "1234567890";
        vga_putc(nums[scancode-0x02]);
    } else if (scancode >= 0x10 && scancode <= 0x19) {
        // q..p
        const char *q = "qwertyuiop";
        vga_putc(q[scancode-0x10]);
    } else if (scancode >= 0x1E && scancode <= 0x26) {
        const char *a = "asdfghjkl";
        vga_putc(a[scancode-0x1E]);
    } else if (scancode >= 0x2C && scancode <= 0x32) {
        const char *z = "zxcvbnm";
        vga_putc(z[scancode-0x2C]);
    } else if (scancode == 0x1C) {
        vga_putc('\n');
    }
    // send EOI to PIC for IRQ1
    __asm__ volatile ("movb $0x20, %%al\n\toutb %%al, $0x20" : : : "al");
}