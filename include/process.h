#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define MAX_TASKS 16
#define PAGE_SIZE 4096

typedef enum {
    TASK_UNUSED = 0,
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_ZOMBIE
} task_state_t;

typedef struct task {
    uint32_t pid;
    task_state_t state;
    uint32_t *stack_ptr;   // saved stack pointer (esp) for context switch
    uint8_t *stack_page;   // allocated stack physical/virtual pointer
} task_t;

void scheduler_init();
int task_create(void (*entry)(void));
void task_yield();
void scheduler_start();

#endif // PROCESS_H
