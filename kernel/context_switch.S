.intel_syntax noprefix
.section .text
.global context_switch
/* cdecl:
   arg1: [esp+4] -> pointer to old_sp storage location (uint32_t *)
   arg2: [esp+8] -> pointer to new_sp storage location (uint32_t *)
*/
context_switch:
    mov eax, dword ptr [esp + 4]   /* eax = &old_sp */
    mov ebx, dword ptr [esp + 8]   /* ebx = &new_sp */

    pushad                         /* save registers of current context */
    mov ecx, esp                   /* ecx = current esp (after pushad) */
    mov dword ptr [eax], ecx       /* *old_sp = current esp */

    mov ecx, dword ptr [ebx]       /* ecx = new_sp (value of tasks[next].stack_ptr) */
    mov esp, ecx                   /* load new esp */

    popad                          /* restore registers for new context */
    ret
