#!/usr/bin/env bash
set -e
OUT=dist
mkdir -p $OUT

nasm -f elf32 boot/boot.asm -o $OUT/boot.o
nasm -f elf32 drivers/keyboard.asm -o $OUT/keyboard_asm.o
nasm -f elf32 kernel/timer_stub.asm -o $OUT/timer_stub.o

clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/kernel.c -o $OUT/kernel.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/interrupts.c -o $OUT/interrupts.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/exceptions.c -o $OUT/exceptions.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/memory.c -o $OUT/memory.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/pmm.c -o $OUT/pmm.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/paging.c -o $OUT/paging.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/timer.c -o $OUT/timer.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c drivers/vga.c -o $OUT/vga.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c drivers/keyboard.c -o $OUT/keyboard.o
# assemble syscall stub
clang --target=i686-elf -m32 -c kernel/isr_syscall.S -o $OUT/isr_syscall.o

# compile syscall handler
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/syscall.c -o $OUT/syscall.o

# multitasking files
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/process.c -o $OUT/process.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/task_examples.c -o $OUT/task_examples.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/context_switch.S -o $OUT/context_switch.o
# ISR stubs compiled previously, ensure isr_stubs.o present in your build (from milestone 2)
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/isr_stubs.S -o $OUT/isr_stubs.o

# gdt & user-mode files assumed compiled already if present
# ... (keep other object lines unchanged)

ld -m elf_i386 -T linker.ld -o $OUT/kernel.bin \
    $OUT/boot.o \
    $OUT/isr_stubs.o \
    $OUT/kernel.o \
    $OUT/interrupts.o \
    $OUT/exceptions.o \
    $OUT/memory.o \
    $OUT/pmm.o \
    $OUT/paging.o \
    $OUT/timer.o \
    $OUT/vga.o \
    $OUT/keyboard.o \
    $OUT/keyboard_asm.o \
    $OUT/timer_stub.o \
    $OUT/context_switch.o \
    $OUT/process.o \
    $OUT/task_examples.o \
    $OUT/isr_syscall.o \
    $OUT/syscall.o \
    # ... add other objs if you have them

echo "Built: $OUT/kernel.bin"
sudo qemu-system-i386 -kernel $OUT/kernel.bin -k en-us
