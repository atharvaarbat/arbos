#include "../include/vga.h"
#include "../include/timer.h"
#include "../include/memory.h"
#include "../include/pmm.h"
#include "../include/paging.h"
// #include "../include/gdt.h"
#include "../include/process.h"
#include "../include/syscall.h"
extern void task_a();
extern void task_b();
void test_syscall_task(void);

void kmain() {
    vga_clear();
    vga_puts("Kernel: initializing...\n");

    pmm_init();
    paging_init();
    init_timer(100);
    // gdt_install(); /* if you have GDT from milestone 2 */

    vga_puts("Scheduler init...\n");
    scheduler_init();

    // create two tasks
    int pid1 = task_create(task_a);
    int pid2 = task_create(task_b);
    // int pid3 = task_create(test_syscall_task);
    test_syscall_task(); // directly call for testing
    vga_puts("Created tasks. Starting scheduler...\n");
    scheduler_start();

    // If scheduler returns, halt
    for (;;) { __asm__ volatile ("hlt"); }
}




void test_syscall_task(void) {
    const char msg[] = "Hello from sys_write!\n";
    sys_write(msg, sizeof(msg)-1);
    for (;;) { __asm__ volatile ("hlt"); }
}