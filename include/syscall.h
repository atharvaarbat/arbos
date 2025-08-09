#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

/* Syscall numbers */
#define SYS_WRITE     1
#define SYS_GET_TICKS 2

/* C wrapper functions for callers */
static inline uint32_t sys_write(const char *buf, uint32_t len) {
    uint32_t ret;
    __asm__ volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(SYS_WRITE), "b"(buf), "c"(len)
        : "memory"
    );
    return ret;
}

static inline uint32_t sys_get_ticks(void) {
    uint32_t ret;
    __asm__ volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(SYS_GET_TICKS)
        : "memory"
    );
    return ret;
}

#endif /* SYSCALL_H */
