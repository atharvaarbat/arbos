#include "../include/interrupts.h"
#include "../include/vga.h"
#include "../include/exceptions.h"
#include <stdint.h>

extern void keyboard_handler();
extern void timer_handler_stub();

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1
#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01
#define ICW4_8086    0x01

struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

#define IDT_ENTRIES 256
static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;

static void set_idt_gate(int n, uint32_t handler) {
    idt[n].base_lo = handler & 0xFFFF;
    idt[n].base_hi = (handler >> 16) & 0xFFFF;
    idt[n].sel = 0x08;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;
}

static void load_idt() {
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;
    __asm__ volatile ("lidt (%0)" : : "r"(&idtp));
}

static void pic_remap() {
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // Unmask IRQ0 (timer) & IRQ1 (keyboard), mask others
    outb(PIC1_DATA, 0xFC); // 11111100
    outb(PIC2_DATA, 0xFF); // 11111111
}

void idt_setup() {
    // Install default exception handlers
    for (int i = 0; i < IDT_ENTRIES; i++) {
        set_idt_gate(i, (uint32_t)exception_handler);
    }

    pic_remap();

    // Install IRQ handlers
    set_idt_gate(32, (uint32_t)timer_handler_stub);
    set_idt_gate(33, (uint32_t)keyboard_handler);

    load_idt();
}
