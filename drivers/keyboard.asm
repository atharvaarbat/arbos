; drivers/keyboard.asm
section .text
global keyboard_handler
extern keyboard_handler_c

keyboard_handler:
    pushad
    cld
    call keyboard_handler_c
    popad
    iret