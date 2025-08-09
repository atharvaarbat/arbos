#include "../include/interrupts.h"
#include "../include/vga.h"
#include "../include/exceptions.h"
#include <stdint.h>

extern void keyboard_handler();
extern void timer_handler_stub();

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void isr_syscall();
extern uint32_t syscall_ret;

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct idt_entry
{
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

#define IDT_ENTRIES 256
static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;

static void set_idt_gate(int n, uint32_t handler)
{
    idt[n].base_lo = handler & 0xFFFF;
    idt[n].base_hi = (handler >> 16) & 0xFFFF;
    idt[n].sel = 0x08;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;
}

static void load_idt()
{
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;
    __asm__ volatile("lidt (%0)" : : "r"(&idtp));
}

static void pic_remap()
{
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

void idt_setup()
{
    // Install default exception handlers
    // for (int i = 0; i < IDT_ENTRIES; i++) {
    //     set_idt_gate(i, (uint32_t)exception_handler);
    // }
    set_idt_gate(0, (uint32_t)isr0);
    set_idt_gate(1, (uint32_t)isr1);
    set_idt_gate(2, (uint32_t)isr2);
    set_idt_gate(3, (uint32_t)isr3);
    set_idt_gate(4, (uint32_t)isr4);
    set_idt_gate(5, (uint32_t)isr5);
    set_idt_gate(6, (uint32_t)isr6);
    set_idt_gate(7, (uint32_t)isr7);
    set_idt_gate(8, (uint32_t)isr8);
    set_idt_gate(9, (uint32_t)isr9);
    set_idt_gate(10, (uint32_t)isr10);
    set_idt_gate(11, (uint32_t)isr11);
    set_idt_gate(12, (uint32_t)isr12);
    set_idt_gate(13, (uint32_t)isr13);
    set_idt_gate(14, (uint32_t)isr14);
    set_idt_gate(15, (uint32_t)isr15);
    set_idt_gate(16, (uint32_t)isr16);
    set_idt_gate(17, (uint32_t)isr17);
    set_idt_gate(18, (uint32_t)isr18);
    set_idt_gate(19, (uint32_t)isr19);
    set_idt_gate(20, (uint32_t)isr20);
    set_idt_gate(21, (uint32_t)isr21);
    set_idt_gate(22, (uint32_t)isr22);
    set_idt_gate(23, (uint32_t)isr23);
    set_idt_gate(24, (uint32_t)isr24);
    set_idt_gate(25, (uint32_t)isr25);
    set_idt_gate(26, (uint32_t)isr26);
    set_idt_gate(27, (uint32_t)isr27);
    set_idt_gate(28, (uint32_t)isr28);
    set_idt_gate(29, (uint32_t)isr29);
    set_idt_gate(30, (uint32_t)isr30);
    set_idt_gate(31, (uint32_t)isr31);

    pic_remap();

    // Install IRQ handlers
    set_idt_gate(32, (uint32_t)timer_handler_stub);
    set_idt_gate(33, (uint32_t)keyboard_handler);

    /* install syscall trap gate at vector 0x80 and allow user-mode to call it (DPL=3) */
    set_idt_gate(0x80, (uint32_t)isr_syscall);
    /* make syscall gate callable from ring 3: set DPL to 3 (bits 5-6 -> 11) */
    idt[0x80].flags = 0xEE; /* present + DPL=3 + 32-bit interrupt gate */
    
    load_idt();
}
