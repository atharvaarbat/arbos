/* Minimal linker script for a Multiboot kernel (ELF32, i386)
 *
 * - Links the kernel at 0x00100000 (1 MiB)
 * - Keeps the multiboot header (.multiboot) at the start of the binary
 * - Provides 'end' symbol for kernel end detection
 * - ENTRY should point to the bootstrap symbol 'start' defined in boot/boot.asm
 */

OUTPUT_FORMAT("elf32-i386")
ENTRY(start)

SECTIONS
{
    /* Link the multiboot header and any data the bootloader expects here.
       Keep it at the very beginning of the linked image. The assembler
       file boot/boot.asm uses `section .multiboot`. */
    . = 0x00100000;

    /* Multiboot header (from boot/boot.asm). Keep its original alignment. */
    .multiboot ALIGN(4) : {
        KEEP(*(.multiboot))
    }

    /* Text segment: kernel code */
    .text ALIGN(4096) : {
        __text_start = .;
        *(.text)           /* .text from all object files */
        *(.text.*)         /* any text.* sections */
        *(.glue_7) *(.glue_7t)
        *(.rodata)         /* read-only data commonly in .rodata */
        __text_end = .;
    }

    /* Read-only data (some toolchains split rodata) */
    .rodata ALIGN(4096) : {
        *(.rodata*)
    }

    /* Data segment */
    .data ALIGN(4096) : {
        __data_start = .;
        *(.data)
        *(.data.*)
        __data_end = .;
    }

    /* BSS (zero-initialized) */
    .bss ALIGN(4096) (NOLOAD) : {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    }

    /* Provide symbol 'end' pointing to the end of the binary image */
    end = .;
    PROVIDE(end = .);

    /* Optional: give convenient symbols */
    PROVIDE(__kernel_start = 0x00100000);
    PROVIDE(__kernel_end = end);
}

/* Default stack size / alignment (for reference only) */
PROVIDE(__stack_size = 16384);
