#include "../include/exceptions.h"
#include "../include/vga.h"

const char *exception_messages[] = {
    "DIVIDE BY ZERO",
    "DEBUG",
    "NMI",
    "BREAKPOINT",
    "OVERFLOW",
    "BOUND RANGE EXCEEDED",
    "INVALID OPCODE",
    "NO COPROCESSOR",
    "DOUBLE FAULT",
    "Coprocessor Segment Overrun",
    "INVALID TSS",
    "SEGMENT NOT PRESENT",
    "STACK SEGMENT FAULT",
    "GENERAL PROTECTION",
    "PAGE FAULT",
    "RESERVED",
    "FLOATING POINT ERROR",
    "ALIGNMENT CHECK",
    "MACHINE CHECK",
    "SIMD FLOATING POINT",
    "VIRTUALIZATION",
    "CONTROL PROTECTION",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED"
};

void exception_handler(int num) {
    vga_puts("\n[EXCEPTION] ");
    if (num < 32) vga_puts(exception_messages[num]);
    else vga_puts("Unknown");
    vga_puts("\nSystem Halted.\n");
    for(;;) asm volatile ("hlt");
}