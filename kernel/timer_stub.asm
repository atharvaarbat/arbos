global timer_handler_stub
extern timer_handler

section .text
timer_handler_stub:
    pushad
    call timer_handler
    popad
    mov al, 0x20
    out 0x20, al
    iret
