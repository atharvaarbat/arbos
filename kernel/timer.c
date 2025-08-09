#include "../include/timer.h"
#include "../include/vga.h"
#include <stdint.h>

static volatile uint32_t ticks = 0;

void timer_handler() {
    ticks++;
    if (ticks % 100 == 0) { // every ~1 sec at 100Hz
        vga_puts(".");
    }
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

#define PIT_COMMAND 0x43 // PIT: Programable Interval Timer
#define PIT_CHANNEL0 0x40
#define PIT_FREQUENCY 1193180

void init_timer(uint32_t frequency) {
    uint32_t divisor = PIT_FREQUENCY / frequency;
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

uint32_t get_ticks() { return ticks; }
