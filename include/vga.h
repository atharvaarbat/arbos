#ifndef VGA_H
#define VGA_H

#include <stdint.h>

void vga_clear();
void vga_putc(char c);
void vga_puts(const char *s);
void vga_puthex(uint32_t x);

#endif // VGA_H