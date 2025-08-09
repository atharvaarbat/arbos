@echo off
setlocal enabledelayedexpansion

set OUT=dist
if not exist %OUT% mkdir %OUT%

nasm -f elf32 boot/boot.asm -o %OUT%/boot.o
nasm -f elf32 drivers/keyboard.asm -o %OUT%/keyboard_asm.o
nasm -f elf32 kernel/timer_stub.asm -o %OUT%/timer_stub.o

clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/kernel.c -o %OUT%/kernel.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/interrupts.c -o %OUT%/interrupts.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/exceptions.c -o %OUT%/exceptions.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/memory.c -o %OUT%/memory.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/pmm.c -o %OUT%/pmm.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/paging.c -o %OUT%/paging.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c kernel/timer.c -o %OUT%/timer.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c drivers/vga.c -o %OUT%/vga.o
clang --target=i686-elf -m32 -ffreestanding -O2 -Iinclude -c drivers/keyboard.c -o %OUT%/keyboard.o

ld -m elf_i386 -T linker.ld -o %OUT%/kernel.bin ^
    %OUT%/boot.o ^
    %OUT%/kernel.o ^
    %OUT%/interrupts.o ^
    %OUT%/exceptions.o ^
    %OUT%/memory.o ^
    %OUT%/pmm.o ^
    %OUT%/paging.o ^
    %OUT%/timer.o ^
    %OUT%/vga.o ^
    %OUT%/keyboard.o ^
    %OUT%/keyboard_asm.o ^
    %OUT%/timer_stub.o

echo Built: %OUT%/kernel.bin
qemu-system-i386 -kernel %OUT%/kernel.bin -k en-us