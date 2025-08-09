.intel_syntax noprefix
.section .text
.global isr_syscall
.global syscall_ret       /* storage symbol used by stub (defined in C) */

.extern syscall_ret

/* Syscall stub for int 0x80
 * Registers on entry:
 *   eax = syscall number
 *   ebx = arg1 (pointer)
 *   ecx = arg2 (length)
 *
 * We'll:
 *   pushad
 *   mov edx, ebx       ; arg1 -> edx (handler expects arg1 in edx)
 *   call syscall_handler_regparm
 *   mov [syscall_ret], eax
 *   popad
 *   mov eax, [syscall_ret]
 *   iret
 *
 * We need syscall_ret to survive popad, so it's global memory.
 */

isr_syscall:
    pushad
    mov edx, ebx
    call syscall_handler_regparm
    /* store return value temporarily */
    mov dword ptr [syscall_ret], eax
    popad
    mov eax, dword ptr [syscall_ret]
    iret
