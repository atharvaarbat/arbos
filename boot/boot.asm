; boot/boot.asm
section .multiboot
align 4
    dd 0x1BADB002      ; Magic number for Multiboot
    dd 0x00            ; Flags
    dd -(0x1BADB002)   ; Checksum

section .text
global start
extern idt_setup     ; defined in kernel/interrupts.c
extern kmain         ; kernel entry point

start:
    cli
    mov esp, stack_top
    call idt_setup     ; create & load IDT, remap PIC
    sti
    call kmain
    cli
.halt:
    hlt
    jmp .halt

section .bss
align 16
stack_bottom:
    resb 16384
stack_top: