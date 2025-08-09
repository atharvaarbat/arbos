#include "../include/vga.h"
#include "../include/process.h"
#include <stdint.h>

void task_a() {
    volatile uint16_t *vga = (volatile uint16_t *)0xB8000;
    int x = 0;
    while (1) {
        vga[80*7 + 10] = (uint16_t)('X' + (x % 26)) | (0x0E << 8);
        for (volatile int i = 0; i < 100000000; ++i) ; // busy work
        task_yield();
        ++x;
    }
}

void task_b() {
    volatile uint16_t *vga = (volatile uint16_t *)0xB8000;
    int x = 0;
    while (1) {
        vga[80*8 + 0] = (uint16_t)('O' + (x % 26)) | (0x0E << 8);
        for (volatile int i = 0; i < 100000000; ++i) ; // busy work
        task_yield();
        ++x;
    }
}
