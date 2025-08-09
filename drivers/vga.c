#include "../include/vga.h"
#include <stdint.h>

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
static uint8_t cursor_x = 0, cursor_y = 0;

static uint16_t make_vga_entry(char c, uint8_t attr) {
    return (uint16_t)c | ((uint16_t)attr << 8);
}

void vga_clear() {
    for (int i = 0; i < 80*25; ++i) vga_buffer[i] = make_vga_entry(' ', 0x07);
    cursor_x = cursor_y = 0;
}

void vga_putc(char c) {
    if (c == '\n') {
        cursor_x = 0; cursor_y++;
    } else {
        vga_buffer[cursor_y*80 + cursor_x] = make_vga_entry(c, 0x07);
        cursor_x++;
        if (cursor_x >= 80) { cursor_x = 0; cursor_y++; }
    }
    if (cursor_y >= 25) {
        // simple scroll: move lines up
        for (int y = 1; y < 25; ++y)
            for (int x = 0; x < 80; ++x)
                vga_buffer[(y-1)*80 + x] = vga_buffer[y*80 + x];
        for (int x = 0; x < 80; ++x)
            vga_buffer[(24)*80 + x] = make_vga_entry(' ', 0x07);
        cursor_y = 24;
    }
}

void vga_puts(const char *s) {
    for (; *s; ++s) vga_putc(*s);
}

void vga_puthex(uint32_t x) {
    const char *hex = "0123456789ABCDEF";
    for (int i = 7; i >= 0; --i) {
        uint8_t nib = (x >> (i*4)) & 0xF;
        vga_putc(hex[nib]);
    }
}