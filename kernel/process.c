#include "../include/process.h"
#include "../include/pmm.h"
#include "../include/vga.h"
#include <stdint.h>
#include <string.h>

static task_t tasks[MAX_TASKS];
static uint32_t next_pid = 1;
static int current = -1;
static int task_count = 0;

/* Forward: context switch implemented in assembly */
extern void context_switch(uint32_t *old_sp_ptr, uint32_t *new_sp_ptr);

/* Internal helper: prepare initial stack for a task.
   We create a stack page and set up a region so that when context_switch
   does popad; ret the task will start at entry.
*/
static uint32_t *prepare_stack(void (*entry)(void), uint8_t *page_addr) {
    // top of stack (stack grows down)
    uint32_t *sp = (uint32_t *)( (uint32_t)page_addr + PAGE_SIZE );

    // Push return EIP first (so it will be popped by ret after popad)
    *(--sp) = (uint32_t)entry; // return address for context switch ret

    // Now push values so that popad will restore registers correctly.
    // popad pops into EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX reading from memory at esp.
    // So memory layout (from low addr at sp upwards) must be:
    // [EDI][ESI][EBP][ESP][EBX][EDX][ECX][EAX] [RET_EIP]
    // We already pushed RET_EIP above; now push EAX..EDI in reverse order:
    *(--sp) = 0; // EAX
    *(--sp) = 0; // ECX
    *(--sp) = 0; // EDX
    *(--sp) = 0; // EBX
    *(--sp) = 0; // ESP (dummy)
    *(--sp) = 0; // EBP
    *(--sp) = 0; // ESI
    *(--sp) = 0; // EDI

    return sp;
}

void scheduler_init() {
    for (int i = 0; i < MAX_TASKS; ++i) {
        tasks[i].pid = 0;
        tasks[i].state = TASK_UNUSED;
        tasks[i].stack_ptr = 0;
        tasks[i].stack_page = 0;
    }
    current = -1;
    task_count = 0;
}

int task_create(void (*entry)(void)) {
    if (task_count >= MAX_TASKS) return -1;

    // find free slot
    int idx = -1;
    for (int i = 0; i < MAX_TASKS; ++i) {
        if (tasks[i].state == TASK_UNUSED) { idx = i; break; }
    }
    if (idx < 0) return -1;

    // allocate one page for stack
    void *p = pmm_alloc_page();
    if (!p) return -1;
    uint8_t *page = (uint8_t *)p;

    // zero the page for clean stack
    for (uint32_t i = 0; i < PAGE_SIZE; ++i) page[i] = 0;

    uint32_t *sp = prepare_stack(entry, page);

    tasks[idx].pid = next_pid++;
    tasks[idx].state = TASK_READY;
    tasks[idx].stack_ptr = sp;
    tasks[idx].stack_page = page;

    task_count++;
    return tasks[idx].pid;
}

static int find_next_ready(int start) {
    if (task_count == 0) return -1;
    int i = (start + 1) % MAX_TASKS;
    for (int cnt = 0; cnt < MAX_TASKS; ++cnt) {
        if (tasks[i].state == TASK_READY) return i;
        i = (i + 1) % MAX_TASKS;
    }
    return -1;
}

void task_yield() {
    if (current < 0) return;
    int next = find_next_ready(current);
    if (next < 0) return; // no other task

    // Mark current as ready
    tasks[current].state = TASK_READY;
    tasks[next].state = TASK_RUNNING;

    uint32_t *old_sp_ptr = (uint32_t *)&tasks[current].stack_ptr;
    uint32_t *new_sp_ptr = (uint32_t *)&tasks[next].stack_ptr;

    int prev = current;
    current = next;
    // context_switch saves current registers to tasks[prev].stack_ptr and loads next
    context_switch(old_sp_ptr, new_sp_ptr);
    // when the task resumes here, it will return back to caller
}

void scheduler_start() {
    // pick first ready task
    int i;
    for (i = 0; i < MAX_TASKS; ++i) {
        if (tasks[i].state == TASK_READY) break;
    }
    if (i == MAX_TASKS) return; // nothing to run

    current = i;
    tasks[i].state = TASK_RUNNING;

    // Switch from kernel main to first task.
    // Create a fake "old" stack pointer variable to save kernel stack pointer.
    uint32_t *kernel_sp_holder = NULL;
    // We need addresses to pass to context_switch: address of old_sp and new_sp
    // For kernel old_sp storage use a local variable on stack
    uint32_t kernel_sp;
    kernel_sp_holder = &kernel_sp;
    // Initialize kernel_sp with current esp so it's saved
    __asm__ volatile ("mov %%esp, %0" : "=r"(kernel_sp) :: );

    // Perform the context switch: save kernel_sp and load first task
    context_switch(kernel_sp_holder, (uint32_t *)&tasks[i].stack_ptr);

    // I only return here when kernel regains control (if ever) :)
}
