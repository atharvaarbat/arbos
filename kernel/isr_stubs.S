.intel_syntax noprefix
.section .text
.set NO_ERRORCODE, 0
.set HAS_ERRORCODE, 1

/* Macro for an ISR without error code:
 *   mov eax, vec
 *   mov edx, 0
 *   pushad
 *   call exception_handler_regparm
 *   popad
 *   iret
 *
 * For an ISR with error code:
 *   mov edx, [esp]      ; read the error code pushed by CPU
 *   add esp, 4          ; remove error code so iret sees EIP/CS/EFLAGS at top
 *   mov eax, vec
 *   pushad
 *   call exception_handler_regparm
 *   popad
 *   iret
 */

.macro ISR_VEC vec, has_err
    .global isr\vec
isr\vec:
    .if \has_err == HAS_ERRORCODE
        mov edx, dword ptr [esp]    /* read error code pushed by CPU */
        add esp, 4                  /* remove error code so iret works */
    .else
        mov edx, 0
    .endif
    mov eax, \vec
    pushad
    call exception_handler_regparm
    popad
    iret
.endm

/* Declare each stub. Vectors 8,10-14,17 have an error code per x86
   (8 = double fault? actually vector 8 pushes an error code), so mark them accordingly.
   According to Intel x86, exceptions that push an error code are:
   8, 10, 11, 12, 13, 14, 17
*/
ISR_VEC 0,  NO_ERRORCODE
ISR_VEC 1,  NO_ERRORCODE
ISR_VEC 2,  NO_ERRORCODE
ISR_VEC 3,  NO_ERRORCODE
ISR_VEC 4,  NO_ERRORCODE
ISR_VEC 5,  NO_ERRORCODE
ISR_VEC 6,  NO_ERRORCODE
ISR_VEC 7,  NO_ERRORCODE
ISR_VEC 8,  HAS_ERRORCODE
ISR_VEC 9,  NO_ERRORCODE
ISR_VEC 10, HAS_ERRORCODE
ISR_VEC 11, HAS_ERRORCODE
ISR_VEC 12, HAS_ERRORCODE
ISR_VEC 13, HAS_ERRORCODE
ISR_VEC 14, HAS_ERRORCODE
ISR_VEC 15, NO_ERRORCODE
ISR_VEC 16, NO_ERRORCODE
ISR_VEC 17, HAS_ERRORCODE
ISR_VEC 18, NO_ERRORCODE
ISR_VEC 19, NO_ERRORCODE
ISR_VEC 20, NO_ERRORCODE
ISR_VEC 21, NO_ERRORCODE
ISR_VEC 22, NO_ERRORCODE
ISR_VEC 23, NO_ERRORCODE
ISR_VEC 24, NO_ERRORCODE
ISR_VEC 25, NO_ERRORCODE
ISR_VEC 26, NO_ERRORCODE
ISR_VEC 27, NO_ERRORCODE
ISR_VEC 28, NO_ERRORCODE
ISR_VEC 29, NO_ERRORCODE
ISR_VEC 30, NO_ERRORCODE
ISR_VEC 31, NO_ERRORCODE

